#include "Collider.h"

#include <magic_enum.hpp>

#include "Core/Debug/Debug.h"
#include "Engine/Entities/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

Collider::Collider(GameObject* _gameObject) : Component(_gameObject), colliderShape(nullptr), rigidActor(nullptr), isTrigger(false), centre(Vector3::Zero)
{
	componentType = ComponentType::Collider;

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
	
	// TODO: Set rigidActor granted rigidbody isn't found on GameObject
}

void Collider::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize ColliderComponent
}

void Collider::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize ColliderComponent
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
