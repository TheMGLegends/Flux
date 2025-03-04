#include "Scene.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"
#include "Engine/Entities/GameObject.h"
#include "Engine/Scene/SceneContext.h"

using namespace Flux;

Scene::Scene()
{
	SceneContext::SetScene(this);

	// INFO: Setup Events to Listen For
	EventDispatcher::AddListener(EventType::GameObjectRemoved, this);

	// TODO: Create a editor camera as well

	// INFO: Create a default play camera
	gameObjects.emplace_back(std::make_unique<GameObject>());
	camera = gameObjects.back().get()->AddComponent<Camera>(gameObjects.back().get());
}

Scene::~Scene()
{
}

void Scene::Serialize(nlohmann::ordered_json& json) const
{
	// TODO: Serialize the type of GameObject and then serialize the GameObject itself

	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Serialize(json);
	}
}

void Scene::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Clear existing game objects and components before "loading" new scene
	gameObjects.clear();
	components.clear();

	// TODO: Get the type of GameObject from json file and instantiate it then pass the json to it

	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Deserialize(json);
	}
}

void Flux::Scene::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
	if (eventType == EventType::GameObjectRemoved)
	{
		auto gameObjectRemovedEvent = std::static_pointer_cast<GameObjectRemovedEvent>(event);

		// INFO: Remove the GameObject from the Scene
		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i].get() == gameObjectRemovedEvent->gameObject)
			{
				gameObjects.erase(gameObjects.begin() + i);
				break;
			}
		}

		// INFO: Remove all now-expired components associated with the removed GameObject
		for (auto& componentList : components)
		{
			componentList.second.erase(
				std::remove_if(componentList.second.begin(), componentList.second.end(), 
			    [](std::weak_ptr<Component> component) { return component.expired(); }), 
			componentList.second.end());
		}
	}
}

void Scene::Start()
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->IsActive())
			gameObjects[i]->Start();
	}
}

void Scene::Update(float deltaTime)
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->IsActive())
			gameObjects[i]->Update(deltaTime);
	}
}

void Scene::LateUpdate(float deltaTime)
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->IsActive())
			gameObjects[i]->LateUpdate(deltaTime);
	}
}

void Scene::FixedUpdate(float fixedDeltaTime)
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->IsActive())
			gameObjects[i]->FixedUpdate(fixedDeltaTime);
	}
}

void Scene::AddComponent(std::weak_ptr<Component> component)
{
	if (component.expired())
	{
		Debug::LogError("Scene::AddComponent() - Component is expired");
		return;
	}

	components[component.lock()->GetComponentType()].push_back(component);
}
