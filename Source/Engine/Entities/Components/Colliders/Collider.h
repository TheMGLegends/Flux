#pragma once

#include "Engine/Entities/Components/Component.h"
#include "Engine/Interfaces/IDebugWireframe.h"

#include <functional>
#include <memory>
#include <SimpleMath.h>
#include <unordered_map>

namespace physx
{
	class PxRigidActor;
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

	enum class RigidActorType
	{
		Static,
		Dynamic
	};

	class Collider : public Component, public IDebugWireframe
	{
	public:
		Collider(GameObject* _gameObject);
		
		virtual void Start() override;
		virtual void SetIsActive(bool _isActive) override;

		void Update(float alpha);

		virtual void Serialize(nlohmann::flux_json& json) const override;
		virtual void Deserialize(const nlohmann::flux_json& json) override;

		void SetIsTrigger(bool _isTrigger);
		inline bool IsTrigger() const { return isTrigger; }

		inline void SetCentre(const DirectX::SimpleMath::Vector3& _centre) { centre = _centre; }
		inline const DirectX::SimpleMath::Vector3& GetCentre() const { return centre; }

		void ExecuteCollisionCallback(CollisionType collisionType, std::shared_ptr<Collider> other);

		/// @brief Sets the rigid actor to be either static or dynamic depending on if a physics body is found on the GameObject
		void SetRigidActor();

		virtual void SetColliderShape() = 0;
		virtual void UpdateScale() = 0;

		inline physx::PxRigidActor* GetRigidActor() const { return rigidActor; }
		inline physx::PxShape& GetColliderShape() const { return *colliderShape; }
		inline RigidActorType GetRigidActorType() const { return rigidActorType; }

	protected:
		physx::PxRigidActor* rigidActor;
		physx::PxShape* colliderShape;

		bool isTrigger;

	private:
		DirectX::SimpleMath::Vector3 centre; // TODO: DOESNT WORK WITH CURRENT IMPLEMENTATION, EITHER DELETE OR FIX

		std::unordered_map<CollisionType, std::function<void(std::shared_ptr<Collider>)>> collisionCallbacks;
		RigidActorType rigidActorType;
	};
}

