#include "Collider.h"

#include <magic_enum.hpp>

#include "Core/Debug/Debug.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

Collider::Collider(GameObject* _gameObject) : Component(_gameObject), rigidActor(nullptr), colliderShape(nullptr), isTrigger(false), 
										      centre(Vector3::Zero), rigidActorType(RigidActorType::Static)
{
	GameObject* gameObject = GetGameObject();

	if (!gameObject)
		Debug::LogError("Collider::Collider() - Collider Component must be attached to a GameObject");

	// INFO: Set Default Collision Callbacks
	collisionCallbacks.try_emplace(CollisionType::CollisionEnter, std::bind(&GameObject::OnCollisionEnter, gameObject, std::placeholders::_1));
	collisionCallbacks.try_emplace(CollisionType::CollisionExit, std::bind(&GameObject::OnCollisionExit, gameObject, std::placeholders::_1));

	collisionCallbacks.try_emplace(CollisionType::TriggerEnter, std::bind(&GameObject::OnTriggerEnter, gameObject, std::placeholders::_1));
	collisionCallbacks.try_emplace(CollisionType::TriggerExit, std::bind(&GameObject::OnTriggerExit, gameObject, std::placeholders::_1));

	// INFO: Set the Rigid Actor
	SetRigidActor();
}

void Collider::Update()
{
	GameObject* gameObject = GetGameObject();
	auto transform = gameObject->transform.lock();

	if (gameObject && transform && rigidActor)
	{
		switch (rigidActorType)
		{
		case RigidActorType::Static:
		{
			// INFO: Granted that it's static we update the actor using transform values
			physx::PxRigidStatic* rigidStatic = static_cast<physx::PxRigidStatic*>(rigidActor);

			const Vector3& position = transform->GetPosition();
			const Quaternion& rotation = transform->GetRotation();

			physx::PxTransform physxTransform(physx::PxVec3(position.x, position.y, position.z),
												physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w));
			rigidStatic->setGlobalPose(physxTransform);

			break;
		}
		case RigidActorType::Dynamic:
		{
			// INFO: Granted that it's dynamic we update the transform using physics simulation values
			physx::PxRigidDynamic* rigidDynamic = static_cast<physx::PxRigidDynamic*>(rigidActor);

			physx::PxTransform physxTransform = rigidDynamic->getGlobalPose();
			transform->SetPosition(Vector3(physxTransform.p.x, physxTransform.p.y, physxTransform.p.z));
			transform->SetRotation(Quaternion(physxTransform.q.x, physxTransform.q.y, physxTransform.q.z, physxTransform.q.w));

			break;
		}
		default:
			break;
		}
	}
}

void Collider::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	auto& jsonBack = json["Components"].back();
	jsonBack["IsTrigger"] = isTrigger;
	jsonBack["Centre"] = { centre.x, centre.y, centre.z };
}

void Collider::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// INFO: Deserialize Collider Data
	isTrigger = json["IsTrigger"].get<bool>();
	centre = Vector3(json["Centre"][0].get<float>(), json["Centre"][1].get<float>(), json["Centre"][2].get<float>());
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
		}
		else
		{
			colliderShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
			colliderShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		}
	}
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
	auto& physics = Physics::GetPhysics();
	GameObject* gameObject = GetGameObject();

	// INFO: Clear RigidActor if it already exists
	if (rigidActor)
	{
		rigidActor->release();
		rigidActor = nullptr;
	}

	if (gameObject)
	{
		const Vector3& position = gameObject->transform.lock()->GetPosition();
		const Quaternion& rotation = gameObject->transform.lock()->GetRotation();

		// INFO: Setup as rigid static actor
		if (!gameObject->HasComponent<PhysicsBody>())
		{
			rigidActor = physics.createRigidStatic(physx::PxTransform(position.x, position.y, position.z,
												   physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
			rigidActorType = RigidActorType::Static;
		}
		else
		{
			rigidActor = physics.createRigidDynamic(physx::PxTransform(position.x, position.y, position.z,
													physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
			rigidActorType = RigidActorType::Dynamic;
		}
	}
}
