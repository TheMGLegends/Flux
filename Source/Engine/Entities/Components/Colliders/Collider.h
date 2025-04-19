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
		explicit Collider(GameObject* _gameObject);
		
		void Start() override;
		void SetIsActive(bool _isActive) override;

		void Update(float alpha);

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		void SetIsTrigger(bool _isTrigger);
		bool IsTrigger() const;

		void SetIsKinematic(bool isKinematic);

		void ExecuteCollisionCallback(CollisionType collisionType, std::shared_ptr<Collider> other);

		/// @brief Sets the rigid actor to be either static or dynamic depending on if a physics body is found on the GameObject
		void SetRigidActor();

		virtual void SetColliderShape() = 0;
		virtual void UpdateScale() = 0;

		physx::PxRigidActor* GetRigidActor() const;
		physx::PxShape& GetColliderShape() const;
		RigidActorType GetRigidActorType() const;

	protected:
		physx::PxRigidActor* rigidActor;
		physx::PxShape* colliderShape;

		bool isTrigger;

	private:
		std::unordered_map<CollisionType, std::function<void(std::shared_ptr<Collider>)>> collisionCallbacks;
		RigidActorType rigidActorType;
	};
}

