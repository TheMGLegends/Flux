#include "PhysicsBody.h"

#include <magic_enum.hpp>

#include "Core/Debug/Debug.h"

using namespace Flux;

PhysicsBody::PhysicsBody(GameObject* _gameObject) : Component(_gameObject), rigidActor(nullptr), mass(1.0f), drag(0.0f), angularDrag(0.05f), useGravity(true)
{
	// INFO: No constraints by default
	for (size_t i = 0; i < static_cast<size_t>(ConstraintAxis::Count); ++i)
	{
		positionConstraints[i] = false;
		rotationConstraints[i] = false;
	}

	// TODO: Search for a collider component and create a rigid dynamic if found and get a reference to it here, set in collider component
}

PhysicsBody::~PhysicsBody()
{
}

void PhysicsBody::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize PhysicsBodyComponent
}

void PhysicsBody::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize PhysicsBodyComponent
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
