#include "GameObject.h"

#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"

using namespace Flux;

GameObject::GameObject() : isActive(true), name("GameObject"), type("GameObject")
{
	transform = AddComponent<Transform>(this);
}

GameObject::~GameObject()
{
}

void GameObject::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize GameObject Data
	json["GameObjects"].push_back({
		{"Name", name},
		{"Type", type},
		{"IsActive", isActive},
		{"Components", nlohmann::json::array()} // INFO: JSON array to store all Components
		});

	// INFO: Get the last Component JSON array
	nlohmann::ordered_json& gameObjectJson = json["GameObjects"].back();

	// INFO: Serialize each Component on the GameObject
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Serialize(gameObjectJson);
	}
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
	std::unique_ptr<GameObject> gameObject;

	// INFO: Default GameObject type creation
	if (typeName == "GameObject")
	{ 
		gameObject = std::make_unique<GameObject>();
		gameObject.get()->SetName("GameObject");
		gameObject.get()->SetType("GameObject");

		return gameObject;
	}

	auto it = gameObjectTypes.find(typeName);
	if (it != gameObjectTypes.end())
	{
		gameObject = it->second();
		gameObject.get()->SetName(typeName);
		gameObject.get()->SetType(typeName);
		return gameObject;
	}

	return nullptr;
}

void GameObject::RegisterGameObjectType(const std::string& typeName, std::function<std::unique_ptr<GameObject>()> creator)
{
	auto result = gameObjectTypes.try_emplace(typeName, std::move(creator));

	if (!result.second)
		Debug::LogError("GameObject::RegisterGameObjectType - GameObject type already registered: " + typeName);
}
