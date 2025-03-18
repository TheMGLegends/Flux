#pragma once

#include "Component.h"

#include <array>

namespace physx { class PxRigidDynamic; }

namespace Flux
{
	enum class ConstraintAxis
	{
		X,
		Y,
		Z,

		Count
	};

	class PhysicsBody : public Component
	{
	public:
		PhysicsBody(GameObject* _gameObject);
		virtual ~PhysicsBody() override;

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		void SetMass(float _mass);
		inline float GetMass() const { return mass; }

		void SetDrag(float _drag);
		inline float GetDrag() const { return drag; }

		void SetAngularDrag(float _angularDrag);
		inline float GetAngularDrag() const { return angularDrag; }

		void SetUseGravity(bool _useGravity);
		inline bool UsesGravity() const { return useGravity; }

		void SetPositionConstraint(bool isConstrained, ConstraintAxis axis);
		inline bool IsPositionConstrained(ConstraintAxis axis) const { return positionConstraints[static_cast<size_t>(axis)]; }

		void SetRotationConstraint(bool isConstrained, ConstraintAxis axis);
		inline bool IsRotationConstrained(ConstraintAxis axis) const { return rotationConstraints[static_cast<size_t>(axis)]; }

	private:
		physx::PxRigidDynamic* rigidDynamic;

		float mass;
		float drag;
		float angularDrag;
		bool useGravity;

		std::array<bool, static_cast<size_t>(ConstraintAxis::Count)> positionConstraints;
		std::array<bool, static_cast<size_t>(ConstraintAxis::Count)> rotationConstraints;
	};
}

