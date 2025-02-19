#include "ColliderComponent.h"

#include "../../GameObject.h"

using namespace DirectX::SimpleMath;

ColliderComponent::ColliderComponent() : colliderShape(nullptr), rigidActor(nullptr)/*TODO: TEMPORARY*/, isTrigger(false), centre(Vector3::Zero)
{
	GameObject* gameObject = GetGameObject();

	if (!gameObject)
	{
		// TODO: Logging System Log Error Message
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

void ColliderComponent::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize ColliderComponent
}

void ColliderComponent::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize ColliderComponent
}

void ColliderComponent::ExecuteCollisionCallback(CollisionType collisionType, std::shared_ptr<ColliderComponent> other)
{
	if (!collisionCallbacks.contains(collisionType))
	{
		// TODO: Logging System Log Error Message
	}

	// INFO: Perform the Collision Callback
	collisionCallbacks[collisionType](other);
}
