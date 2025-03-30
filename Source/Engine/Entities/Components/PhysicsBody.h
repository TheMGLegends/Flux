#pragma once

#include "Component.h"

#include <array>
#pragma warning (push, 0)
#include <PxForceMode.h>
#pragma warning (pop)

namespace physx { class PxRigidDynamic; }
namespace DirectX::SimpleMath { struct Vector3; }

namespace Flux
{
	enum class ConstraintAxis
	{
		X,
		Y,
		Z,

		Count
	};

	class Collider;

	class PhysicsBody : public Component
	{
	public:
		PhysicsBody(GameObject* _gameObject);
		virtual ~PhysicsBody() override;

		virtual void PostConstruction() override;

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		void AddForce(const DirectX::SimpleMath::Vector3& force, physx::PxForceMode::Enum forceMode = physx::PxForceMode::eFORCE);

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

		physx::PxRigidDynamic* VerifyRigidActor();

	private:
		std::weak_ptr<Collider> attachedCollider;

		float mass;
		float drag;
		float angularDrag;
		bool useGravity;

		std::array<bool, static_cast<size_t>(ConstraintAxis::Count)> positionConstraints;
		std::array<bool, static_cast<size_t>(ConstraintAxis::Count)> rotationConstraints;
	};
}

