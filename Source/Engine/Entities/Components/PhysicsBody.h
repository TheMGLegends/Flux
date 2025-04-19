#pragma once

#include "Component.h"

#include <array>
#include <memory>
#include <PxForceMode.h>

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

	class PhysicsBody : public Component, public std::enable_shared_from_this<PhysicsBody>
	{
	public:
		explicit PhysicsBody(GameObject* _gameObject);
		~PhysicsBody() override;

		void PostConstruction() override;
		void PostDestruction() override;
		void Start() override;
		void DrawDetails() override;

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		void SetIsActive(bool _isActive) override;

		void AddForce(const DirectX::SimpleMath::Vector3& force, physx::PxForceMode::Enum forceMode = physx::PxForceMode::eFORCE);
		void AddTorque(const DirectX::SimpleMath::Vector3& torque, physx::PxForceMode::Enum forceMode = physx::PxForceMode::eFORCE);

		void SetMass(float _mass);
		float GetMass() const;

		void SetDrag(float _drag);
		float GetDrag() const;

		void SetAngularDrag(float _angularDrag);
		float GetAngularDrag() const;

		void SetUseGravity(bool _useGravity);
		bool UsesGravity() const;

		void SetIsKinematic(bool _isKinematic);
		bool IsKinematic() const;

		void SetPositionConstraint(bool isConstrained, ConstraintAxis axis);
		bool IsPositionConstrained(ConstraintAxis axis) const;

		void SetRotationConstraint(bool isConstrained, ConstraintAxis axis);
		bool IsRotationConstrained(ConstraintAxis axis) const;

		physx::PxRigidDynamic* VerifyRigidActor();

	private:
		/// @return true if the field was changed
		bool DisplayArray3Field(const char* label, bool* array) const;

	private:
		std::weak_ptr<Collider> attachedCollider;

		float mass;
		float drag;
		float angularDrag;
		bool useGravity;
		bool isKinematic;

		std::array<bool, static_cast<size_t>(ConstraintAxis::Count)> positionConstraints;
		std::array<bool, static_cast<size_t>(ConstraintAxis::Count)> rotationConstraints;
	};
}

