#include "Collider.h"

#include <magic_enum.hpp>

#include "Core/Debug/Debug.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

Collider::Collider(GameObject* _gameObject) : Component(_gameObject), colliderShape(nullptr), rigidStatic(nullptr), isTrigger(false), centre(Vector3::Zero)
{
	GameObject* gameObject = GetGameObject();

	if (!gameObject)
	{
		Debug::LogError("Collider::Collider() - Collider Component must be attached to a GameObject");
	}

	// INFO: Set Default Collision Callbacks
	collisionCallbacks.try_emplace(CollisionType::CollisionEnter, std::bind(&GameObject::OnCollisionEnter, gameObject, std::placeholders::_1));
	collisionCallbacks.try_emplace(CollisionType::CollisionStay, std::bind(&GameObject::OnCollisionStay, gameObject, std::placeholders::_1));
	collisionCallbacks.try_emplace(CollisionType::CollisionExit, std::bind(&GameObject::OnCollisionExit, gameObject, std::placeholders::_1));

	collisionCallbacks.try_emplace(CollisionType::TriggerEnter, std::bind(&GameObject::OnTriggerEnter, gameObject, std::placeholders::_1));
	collisionCallbacks.try_emplace(CollisionType::TriggerStay, std::bind(&GameObject::OnTriggerStay, gameObject, std::placeholders::_1));
	collisionCallbacks.try_emplace(CollisionType::TriggerExit, std::bind(&GameObject::OnTriggerExit, gameObject, std::placeholders::_1));
}

void Collider::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	auto& jsonBack = json["Components"].back();
	jsonBack["IsTrigger"] = isTrigger;
	jsonBack["Centre"] = { centre.x, centre.y, centre.z };
}

void Collider::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// INFO: Deserialize Collider Data
	isTrigger = json["IsTrigger"].get<bool>();
	centre = Vector3(json["Centre"][0].get<float>(), json["Centre"][1].get<float>(), json["Centre"][2].get<float>());
}

void Collider::ExecuteCollisionCallback(CollisionType collisionType, std::shared_ptr<Collider> other)
{
	if (!collisionCallbacks.contains(collisionType))
	{
		Debug::LogError("Collider::ExecuteCollisionCallback() - Collision Callback not found for Collision Type: " + std::string{ magic_enum::enum_name(collisionType) });
	}

	// INFO: Perform the Collision Callback
	collisionCallbacks[collisionType](other);
}
