#include "Scene.h"

#include <magic_enum.hpp>

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"
#include "Engine/Entities/GameObject.h"
#include "Engine/Scene/SceneContext.h"

// TODO: TESTING
#include "Engine/Entities/Components/Visualizer.h"
#include "Engine/Entities/Components/Colliders/BoxCollider.h"
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

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

	// TODO: Testing
	gameObjects.emplace_back(std::make_unique<GameObject>());
	gameObjects.back().get()->AddComponent<Visualizer>(gameObjects.back().get());
	auto transform = gameObjects.back().get()->GetComponent<Transform>().lock();
	transform->SetPosition(Vector3(0.0f, 0.0f, 3.0f));
	gameObjects.back().get()->AddComponent<BoxCollider>(gameObjects.back().get());
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

		// INFO: Remove all now-expired debug wireframes associated with the removed GameObject
		debugWireframes.erase(
			std::remove_if(debugWireframes.begin(), debugWireframes.end(),
			[](const DebugWireframeData& data) { return data.component.expired(); }),
		debugWireframes.end());
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

void Flux::Scene::DrawWireframes(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
{
	for (size_t i = 0; i < debugWireframes.size(); i++)
	{
		if (debugWireframes[i].component.expired())
			continue;

		auto underlyingComponent = debugWireframes[i].component.lock();

		if (!underlyingComponent->IsActive() || !underlyingComponent->GetGameObject()->IsActive())
			continue;

		debugWireframes[i].debugWireframe->DrawWireframe(deviceContext, primitiveBatch);
	}
}

void Scene::RegisterComponent(std::weak_ptr<Component> component)
{
	if (component.expired())
	{
		Debug::LogError("Scene::RegisterComponent() - Component is expired");
		return;
	}

	auto validComponent = component.lock();

	switch (validComponent->GetComponentType())
	{
	// INFO: Both inherit from IDebugWireframe
	case ComponentType::Camera:
	case ComponentType::Collider:
	{

		std::weak_ptr<IDebugWireframe> debugWireframe = std::dynamic_pointer_cast<IDebugWireframe>(validComponent);

		if (!debugWireframe.expired())
		{
			DebugWireframeData debugWireframeData(component, debugWireframe.lock().get());
			debugWireframes.push_back(debugWireframeData);
		}
		else
			Debug::LogError("Scene::RegisterComponent() - Component could not be added to IDebugWireframe container. Component Type: " + std::string(magic_enum::enum_name(validComponent->GetComponentType())));
		break;
	}
	default:
		break;
	}

	components[validComponent->GetComponentType()].push_back(component);
}
