#include "PhysicsBody.h"

#include <magic_enum.hpp>
#include <extensions/PxRigidBodyExt.h>

#include "Core/Debug/Debug.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/Components/Colliders/BoxCollider.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

PhysicsBody::PhysicsBody(GameObject* _gameObject) : Component(_gameObject), mass(1.0f), drag(0.0f), angularDrag(0.05f), useGravity(true)
{
	name = "PhysicsBody";
	componentType = ComponentType::PhysicsBody;

	// INFO: No constraints by default
	for (size_t i = 0; i < static_cast<size_t>(ConstraintAxis::Count); ++i)
	{
		positionConstraints[i] = false;
		rotationConstraints[i] = false;
	}
}

PhysicsBody::~PhysicsBody()
{
}

void PhysicsBody::PostConstruction()
{
	GameObject* gameObject = GetGameObject();

	if (gameObject)
	{
		// INFO: Add Default BoxCollider if no Collider is found
		if (!gameObject->HasComponent<Collider>())
		{
			Debug::LogWarning("PhysicsBody::PhysicsBody() - No collider found on GameObject, adding a default BoxCollider");
			attachedCollider = gameObject->AddComponent<BoxCollider>(gameObject);
		}
		else
		{
			attachedCollider = gameObject->GetComponent<Collider>();

			// INFO: If collider was already present, it defaulted to static so we need to set it to dynamic
			if (!attachedCollider.expired())
			{
				std::shared_ptr<Collider> collider = attachedCollider.lock();

				// INFO: Reset the Rigid Actor to be dynamic
				collider->SetRigidActor();

				// INFO: Reset the Collider Shape to be attached to the new Rigid Actor
				collider->SetColliderShape();
			}
		}

		// INFO: Set RigidActor Properties
		SetMass(mass);
		SetDrag(drag);
		SetAngularDrag(angularDrag);
	}
}

void PhysicsBody::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	auto& jsonBack = json["Components"].back();
	jsonBack["Mass"] = mass;
	jsonBack["Drag"] = drag;
	jsonBack["AngularDrag"] = angularDrag;
	jsonBack["UseGravity"] = useGravity;
	jsonBack["PositionConstraints"] = { positionConstraints[0], positionConstraints[1], positionConstraints[2] };
	jsonBack["RotationConstraints"] = { rotationConstraints[0], rotationConstraints[1], rotationConstraints[2] };
}

void PhysicsBody::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// INFO: Deserialize PhysicsBody Data
	SetMass(json["Mass"].get<float>());
	SetDrag(json["Drag"].get<float>());
	SetAngularDrag(json["AngularDrag"].get<float>());
	SetUseGravity(json["UseGravity"].get<bool>());

	auto& positionConstraintsJson = json["PositionConstraints"];
	for (size_t i = 0; i < positionConstraintsJson.size(); ++i)
	{
		SetPositionConstraint(positionConstraintsJson[i].get<bool>(), static_cast<ConstraintAxis>(i));
	}

	auto& rotationConstraintsJson = json["RotationConstraints"];
	for (size_t i = 0; i < rotationConstraintsJson.size(); ++i)
	{
		SetRotationConstraint(rotationConstraintsJson[i].get<bool>(), static_cast<ConstraintAxis>(i));
	}
}

void PhysicsBody::SetMass(float _mass)
{
	mass = _mass;

	auto rigidDynamic = VerifyRigidActor();

	if (rigidDynamic)
		physx::PxRigidBodyExt::updateMassAndInertia(*rigidDynamic, mass);
}

void PhysicsBody::SetDrag(float _drag)
{
	drag = _drag;

	auto rigidDynamic = VerifyRigidActor();

	if (rigidDynamic)
		rigidDynamic->setLinearDamping(drag);
}

void PhysicsBody::SetAngularDrag(float _angularDrag)
{
	angularDrag = _angularDrag;

	auto rigidDynamic = VerifyRigidActor();

	if (rigidDynamic)
		rigidDynamic->setAngularDamping(angularDrag);
}

void PhysicsBody::SetUseGravity(bool _useGravity)
{
	useGravity = _useGravity;

	auto rigidDynamic = VerifyRigidActor();

	if (rigidDynamic)
		rigidDynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !useGravity);
}

void PhysicsBody::SetPositionConstraint(bool isConstrained, ConstraintAxis axis)
{
	if (axis == ConstraintAxis::Count)
	{
		Debug::LogError("PhysicsBody::SetPositionConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
		return;
	}

	positionConstraints[static_cast<size_t>(axis)] = isConstrained;

	auto rigidDynamic = VerifyRigidActor();

	if (rigidDynamic)
	{
		rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, positionConstraints[0]); // INFO: X-Axis
		rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, positionConstraints[1]); // INFO: Y-Axis
		rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, positionConstraints[2]); // INFO: Z-Axis
	}
}

void PhysicsBody::SetRotationConstraint(bool isConstrained, ConstraintAxis axis)
{
	if (axis == ConstraintAxis::Count)
	{
		Debug::LogError("PhysicsBody::SetRotationConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
		return;
	}

	rotationConstraints[static_cast<size_t>(axis)] = isConstrained;

	auto rigidDynamic = VerifyRigidActor();

	if (rigidDynamic)
	{
		rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rotationConstraints[0]); // INFO: X-Axis
		rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rotationConstraints[1]); // INFO: Y-Axis
		rigidDynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rotationConstraints[2]); // INFO: Z-Axis
	}
}

physx::PxRigidDynamic* PhysicsBody::VerifyRigidActor()
{
	if (auto collider = attachedCollider.lock())
	{
		if (collider->GetRigidActorType() == RigidActorType::Dynamic)
			return static_cast<physx::PxRigidDynamic*>(collider->GetRigidActor());
	}

	Debug::LogError("PhysicsBody::VerifyRigidActor() - No Collider found or Collider is not dynamic");
	return nullptr;
}
