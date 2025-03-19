#pragma once

#include "Engine/Entities/Components/Component.h"
#include "Engine/Interfaces/IDebugWireframe.h"

#include <functional>
#include <memory>
#include <SimpleMath.h>
#include <unordered_map>

namespace physx
{
	class PxRigidStatic;
	class PxShape;
}

namespace Flux
{
	enum class CollisionType
	{
		CollisionEnter,
		CollisionExit,

		TriggerEnter,
		TriggerExit,

		Count
	};

	class Collider : public Component, public IDebugWireframe
	{
	public:
		Collider(GameObject* _gameObject);

		/// @brief Update the rigid static actor based on transform values granted it is being used (No PhysicsBody)
		void Update();

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		void SetIsTrigger(bool _isTrigger);
		inline bool IsTrigger() const { return isTrigger; }

		inline void SetCentre(const DirectX::SimpleMath::Vector3& _centre) { centre = _centre; }
		inline const DirectX::SimpleMath::Vector3& GetCentre() const { return centre; }

		void ExecuteCollisionCallback(CollisionType collisionType, std::shared_ptr<Collider> other);

		inline physx::PxShape& GetColliderShape() const { return *colliderShape; }
		inline physx::PxRigidStatic& GetRigidStatic() const { return *rigidStatic; }

	protected:
		physx::PxShape* colliderShape;
		physx::PxRigidStatic* rigidStatic;

	private:
		bool isTrigger;
		DirectX::SimpleMath::Vector3 centre;

		std::unordered_map<CollisionType, std::function<void(std::shared_ptr<Collider>)>> collisionCallbacks;
	};
}

