#include "GameObject.h"

#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"

using namespace Flux;

GameObject::GameObject() : isActive(true)
{
	transform = AddComponent<Transform>(this);
}

GameObject::~GameObject()
{
}

void GameObject::Serialize(nlohmann::ordered_json& json) const
{
}

void GameObject::Deserialize(const nlohmann::ordered_json& json)
{
}

void GameObject::SetIsActive(bool _isActive)
{
	// INFO: Immediate change in active state
	isActive = _isActive;

	// INFO: Handle any OnEnable/OnDisable logic here
	if (isActive)
	{
		OnEnable();
		// TODO: Notify Event System of recent enabling of GO, so that it's start can be called at the start of the next frame
	}
	else
		OnDisable();
}

void GameObject::Destroy()
{
	// INFO: Handle any OnDestroy logic here
	OnDestroy();

	EventDispatcher::QueueEvent(EventType::GameObjectRemoved, std::make_shared<GameObjectRemovedEvent>(this));

	SetIsActive(false);
}

std::unique_ptr<GameObject> GameObject::CreateGameObject(const std::string& typeName)
{
	auto it = gameObjectTypes.find(typeName);
	return it != gameObjectTypes.end() ? it->second() : nullptr;
}

void GameObject::RegisterGameObjectType(const std::string& typeName, std::function<std::unique_ptr<GameObject>()> creator)
{
	auto result = gameObjectTypes.try_emplace(typeName, std::move(creator));

	if (!result.second)
		Debug::LogError("GameObject::RegisterGameObjectType - GameObject type already registered: " + typeName);
}
