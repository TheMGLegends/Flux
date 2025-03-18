#include "PhysicsBody.h"

#include <magic_enum.hpp>

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

	// INFO: Search for existing collider component
	GameObject* owningGameObject = GetGameObject();

	if (owningGameObject)
	{
		// INFO: Setup rigid dynamic actor
		auto& physics = Physics::GetPhysics();
		auto& physicsScene = SceneContext::GetScene().GetPhysicsScene();

		const Vector3& position = owningGameObject->transform.lock()->GetPosition();
		const Quaternion& rotation = owningGameObject->transform.lock()->GetRotation();
		rigidDynamic = physics.createRigidDynamic(physx::PxTransform(position.x, position.y, position.z,
												  physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));

		std::shared_ptr<Collider> collider;
		bool hadCollider = false;

		if (!owningGameObject->HasComponent<Collider>())
		{
			Debug::LogWarning("PhysicsBody::PhysicsBody() - No collider found on GameObject, adding a default BoxCollider");
			collider = owningGameObject->AddComponent<BoxCollider>(owningGameObject).lock();
		}
		else
		{
			hadCollider = true;
			collider = owningGameObject->GetComponent<Collider>().lock();
		}

		// INFO: Attach Collider Shape to Rigid Dynamic Actor
		rigidDynamic->attachShape(collider->GetColliderShape());

		// INFO: Remove the Rigid Static Actor from the Physics Scene (If it existed beforehand)
		if (hadCollider)
			physicsScene.removeActor(collider->GetRigidStatic());

		// INFO: Add the Rigid Dynamic Actor to the Physics Scene
		physicsScene.addActor(*rigidDynamic);
	}
}

PhysicsBody::~PhysicsBody()
{
	if (rigidDynamic)
	{
		rigidDynamic->release();
		rigidDynamic = nullptr;
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
	
	// TODO: Set Mass on RigidActor
}

void PhysicsBody::SetDrag(float _drag)
{
	drag = _drag;

	// TODO: Set Drag on RigidActor
}

void PhysicsBody::SetAngularDrag(float _angularDrag)
{
	angularDrag = _angularDrag;

	// TODO: Set Angular Drag on RigidActor
}

void PhysicsBody::SetUseGravity(bool _useGravity)
{
	useGravity = _useGravity;

	// TODO: Set Use Gravity on RigidActor
}

void PhysicsBody::SetPositionConstraint(bool isConstrained, ConstraintAxis axis)
{
	if (axis == ConstraintAxis::Count)
	{
		Debug::LogError("PhysicsBody::SetPositionConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
		return;
	}

	positionConstraints[static_cast<size_t>(axis)] = isConstrained;

	// TODO: Set Position Constraint on RigidActor
}

void PhysicsBody::SetRotationConstraint(bool isConstrained, ConstraintAxis axis)
{
	if (axis == ConstraintAxis::Count)
	{
		Debug::LogError("PhysicsBody::SetRotationConstraint() - Invalid Constraint Axis: " + std::string{ magic_enum::enum_name(axis) });
		return;
	}

	rotationConstraints[static_cast<size_t>(axis)] = isConstrained;

	// TODO: Set Rotation Constraint on RigidActor
}
