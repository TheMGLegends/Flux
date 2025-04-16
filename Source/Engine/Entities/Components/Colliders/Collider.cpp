#include "Collider.h"

#include <magic_enum.hpp>

#include "Core/Debug/Debug.h"

#include "Engine/Physics/Physics.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"

using namespace DirectX::SimpleMath;

namespace Flux 
{
	Collider::Collider(GameObject* _gameObject) : Component(_gameObject), rigidActor(nullptr), colliderShape(nullptr), 
												  isTrigger(false), rigidActorType(RigidActorType::Static)
	{
		GameObject* gameObject = GetGameObject();

		if (!gameObject)
		{
			Debug::LogError("Collider::Collider() - Collider Component must be attached to a GameObject");
		}

		// INFO: Set Default Collision Callbacks
		using enum Flux::CollisionType;
		collisionCallbacks.try_emplace(CollisionEnter, std::bind_front(&GameObject::OnCollisionEnter, gameObject));
		collisionCallbacks.try_emplace(CollisionExit, std::bind_front(&GameObject::OnCollisionExit, gameObject));

		collisionCallbacks.try_emplace(TriggerEnter, std::bind_front(&GameObject::OnTriggerEnter, gameObject));
		collisionCallbacks.try_emplace(TriggerExit, std::bind_front(&GameObject::OnTriggerExit, gameObject));

		// INFO: Set the Rigid Actor
		SetRigidActor();
	}

	void Collider::Start()
	{
		SetIsActive(isActive);
		SetIsTrigger(isTrigger);

		// INFO: Set global pose for rigid actor
		std::shared_ptr<Transform> transform = GetGameObject()->GetComponent<Transform>().lock();

		if (transform)
		{
			Vector3 position = transform->GetPosition();
			Quaternion rotation = transform->GetRotation();

			physx::PxTransform physxTransform(physx::PxVec3(position.x, position.y, position.z),
											  physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
		}
	}

	void Collider::SetIsActive(bool _isActive)
	{
		Component::SetIsActive(_isActive);

		if (colliderShape && rigidActor)
		{
			if (isActive)
			{
				rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, false);

				if (rigidActorType == RigidActorType::Dynamic)
				{
					auto rigidDynamic = static_cast<physx::PxRigidDynamic*>(rigidActor);
					rigidDynamic->wakeUp();
				}
			}
			else
			{
				rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_SIMULATION, true);
			}
		}
	}

	void Collider::Update(float alpha)
	{
		const GameObject* gameObject = GetGameObject();
		std::shared_ptr<Transform> transform = gameObject->transform.lock();

		if (!gameObject || !gameObject->IsActive() || !isActive) { return; }

		if (transform && rigidActor)
		{
			switch (rigidActorType)
			{
			case RigidActorType::Static:
			{
				// INFO: Granted that it's static we update the actor using transform values
				if (auto rigidStatic = static_cast<physx::PxRigidStatic*>(rigidActor); rigidStatic)
				{
					const Vector3& position = transform->GetPosition();
					const Quaternion& rotation = transform->GetRotation();

					physx::PxTransform physxTransform(physx::PxVec3(position.x, position.y, position.z),
						physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
					rigidStatic->setGlobalPose(physxTransform);
				}

				break;
			}
			case RigidActorType::Dynamic:
			{
				// INFO: Granted that it's dynamic we update the transform using physics simulation values
				if (auto rigidDynamic = static_cast<physx::PxRigidDynamic*>(rigidActor); rigidDynamic)
				{
					physx::PxTransform physxTransform = rigidDynamic->getGlobalPose();
					Vector3 currentPosition = transform->GetPosition();
					Quaternion currentRotation = transform->GetRotation();

					Vector3 newPosition(physxTransform.p.x, physxTransform.p.y, physxTransform.p.z);
					Quaternion newRotation(physxTransform.q.x, physxTransform.q.y, physxTransform.q.z, physxTransform.q.w);

					// INFO: Lerp the position and rotation to avoid jittering
					transform->SetPosition(Vector3::Lerp(currentPosition, newPosition, alpha));
					transform->SetRotation(Quaternion::Slerp(currentRotation, newRotation, alpha));
				}

				break;
			}
			default:
				break;
			}
		}
	}

	void Collider::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize Parent Class
		Component::Serialize(json);

		auto& jsonBack = json["Components"].back();
		jsonBack["IsTrigger"] = isTrigger;
	}

	void Collider::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize Parent Class
		Component::Deserialize(json);

		// INFO: Deserialize Collider Data
		isTrigger = json["IsTrigger"].get<bool>();
	}

	void Collider::SetIsTrigger(bool _isTrigger)
	{
		isTrigger = _isTrigger;

		if (colliderShape)
		{
			if (isTrigger)
			{
				colliderShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
				colliderShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);

				if (rigidActorType != RigidActorType::Dynamic) { return; }

				// INFO: Disable gravity and prevent physics movement by changing to kinematic
				auto rigidDynamic = static_cast<physx::PxRigidDynamic*>(rigidActor);
				rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
				rigidDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
			}
			else
			{
				colliderShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
				colliderShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

				if (rigidActorType != RigidActorType::Dynamic) { return; }

				// INFO: Re-enable gravity and physics movement
				auto rigidDynamic = static_cast<physx::PxRigidDynamic*>(rigidActor);
				rigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);

				std::shared_ptr<PhysicsBody> physicsBody = GetGameObject()->GetComponent<PhysicsBody>().lock();

				// INFO: Only enable gravity if theres an associated PhysicsBody that uses gravity
				if (physicsBody && physicsBody->UsesGravity())
				{
					rigidDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
				}
			}
		}
	}

	bool Collider::IsTrigger() const
	{
		return isTrigger;
	}

	void Collider::ExecuteCollisionCallback(CollisionType collisionType, std::shared_ptr<Collider> other)
	{
		if (!collisionCallbacks.contains(collisionType))
		{
			Debug::LogError("Collider::ExecuteCollisionCallback() - Collision Callback not found for Collision Type: " + std::string{ magic_enum::enum_name(collisionType) });
		}

		// INFO: Perform the Collision Callback
		collisionCallbacks[collisionType](other);
	}

	void Collider::SetRigidActor()
	{
		physx::PxPhysics& physics = Physics::GetPhysics();
		GameObject* gameObject = GetGameObject();

		bool existed = false;

		std::weak_ptr<Collider> collider = SceneContext::GetScene().GetCollider(rigidActor);

		// INFO: Clear RigidActor if it already exists
		if (rigidActor)
		{
			existed = true;

			// INFO: Remove Entry corresponding to Rigid Actor from Scene of rigidActorsToColliders before setting new Rigid Actor
			SceneContext::GetScene().RemoveRigidActorToColliderEntry(rigidActor);

			rigidActor->release();
			rigidActor = nullptr;
		}

		if (gameObject)
		{
			const Vector3& position = gameObject->transform.lock()->GetPosition();
			const Quaternion& rotation = gameObject->transform.lock()->GetRotation();

			// INFO: Setup as rigid static actor
			if (std::shared_ptr<PhysicsBody> physicsBody = gameObject->GetComponent<PhysicsBody>().lock(); !physicsBody || !physicsBody->IsActive())
			{
				rigidActor = physics.createRigidStatic(physx::PxTransform(position.x, position.y, position.z,
					physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
				rigidActorType = RigidActorType::Static;
			}
			// INFO: Setup as rigid dynamic actor
			else
			{
				rigidActor = physics.createRigidDynamic(physx::PxTransform(position.x, position.y, position.z,
					physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
				rigidActorType = RigidActorType::Dynamic;
			}

			if (existed && !collider.expired())
			{
				// INFO: Register the newly created Rigid Actor to the Collider
				SceneContext::GetScene().RegisterRigidActorToCollider(collider, rigidActor);
			}
		}
	}

	physx::PxRigidActor* Collider::GetRigidActor() const
	{
		return rigidActor;
	}

	physx::PxShape& Collider::GetColliderShape() const
	{
		return *colliderShape;
	}

	RigidActorType Collider::GetRigidActorType() const
	{
		return rigidActorType;
	}
}
