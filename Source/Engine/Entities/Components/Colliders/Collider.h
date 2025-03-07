#pragma once

#include "Engine/Entities/Components/Component.h"
#include "Engine/Interfaces/IDebugWireframe.h"

#include <functional>
#include <memory>
#include <SimpleMath.h>
#include <unordered_map>

class PxRigidActor;
class PxShape;

namespace Flux
{
	enum class CollisionType
	{
		CollisionEnter,
		CollisionStay,
		CollisionExit,

		TriggerEnter,
		TriggerStay,
		TriggerExit,

		Count
	};

	class Collider : public Component, public IDebugWireframe
	{
	public:
		Collider(GameObject* _gameObject);

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		inline void SetIsTrigger(bool _isTrigger) { isTrigger = _isTrigger; }
		inline bool IsTrigger() const { return isTrigger; }

		inline void SetCentre(const DirectX::SimpleMath::Vector3& _centre) { centre = _centre; }
		inline const DirectX::SimpleMath::Vector3& GetCentre() const { return centre; }

		void ExecuteCollisionCallback(CollisionType collisionType, std::shared_ptr<Collider> other);

	protected:
		PxShape* colliderShape;

	private:
		PxRigidActor* rigidActor;

		bool isTrigger;
		DirectX::SimpleMath::Vector3 centre;

		std::unordered_map<CollisionType, std::function<void(std::shared_ptr<Collider>)>> collisionCallbacks;
	};
}

