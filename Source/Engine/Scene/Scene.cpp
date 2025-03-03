#include "Scene.h"

#include "Engine/Entities/GameObject.h"
#include "Engine/Entities/Components/Camera.h"

using namespace Flux;

Scene::Scene()
{
	// TODO: Create a editor camera as well

	// INFO: Create a default play camera
	gameObjects.emplace_back(std::make_unique<GameObject>());
	camera = gameObjects.back().get()->AddComponent<Camera>();
}

Scene::~Scene()
{
}

void Scene::Serialize(nlohmann::ordered_json& json) const
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Serialize(json);
	}
}

void Scene::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Clear existing game objects
	gameObjects.clear();

	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Deserialize(json);
	}
}

void Scene::Start()
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Start();
	}
}

void Scene::Update(float deltaTime)
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Update(deltaTime);
	}
}

void Scene::LateUpdate(float deltaTime)
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->LateUpdate(deltaTime);
	}
}

void Scene::FixedUpdate(float fixedDeltaTime)
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->FixedUpdate(fixedDeltaTime);
	}
}
