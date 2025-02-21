#include "PhysicsBodyComponent.h"

PhysicsBodyComponent::PhysicsBodyComponent() : rigidActor(nullptr), mass(1.0f), drag(0.0f), angularDrag(0.05f), useGravity(true)
{
	// INFO: No constraints by default
	for (size_t i = 0; i < static_cast<size_t>(ConstraintAxis::Count); ++i)
	{
		positionConstraints[i] = false;
		rotationConstraints[i] = false;
	}

	// TODO: Search for a collider component and create a rigid dynamic if found and get a reference to it here, set in collider component
}

PhysicsBodyComponent::~PhysicsBodyComponent()
{
}

void PhysicsBodyComponent::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize PhysicsBodyComponent
}

void PhysicsBodyComponent::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize PhysicsBodyComponent
}

void PhysicsBodyComponent::SetMass(float _mass)
{
	mass = _mass;
	
	// TODO: Set Mass on RigidActor
}

void PhysicsBodyComponent::SetDrag(float _drag)
{
	drag = _drag;

	// TODO: Set Drag on RigidActor
}

void PhysicsBodyComponent::SetAngularDrag(float _angularDrag)
{
	angularDrag = _angularDrag;

	// TODO: Set Angular Drag on RigidActor
}

void PhysicsBodyComponent::SetUseGravity(bool _useGravity)
{
	useGravity = _useGravity;

	// TODO: Set Use Gravity on RigidActor
}

void PhysicsBodyComponent::SetPositionConstraint(bool isConstrained, ConstraintAxis axis)
{
	if (axis == ConstraintAxis::Count)
	{
		// TODO: Logging System Log Error Message
	}

	positionConstraints[static_cast<size_t>(axis)] = isConstrained;

	// TODO: Set Position Constraint on RigidActor
}

void PhysicsBodyComponent::SetRotationConstraint(bool isConstrained, ConstraintAxis axis)
{
	if (axis == ConstraintAxis::Count)
	{
		// TODO: Logging System Log Error Message
	}

	rotationConstraints[static_cast<size_t>(axis)] = isConstrained;

	// TODO: Set Rotation Constraint on RigidActor
}
