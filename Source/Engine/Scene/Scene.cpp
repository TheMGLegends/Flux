#include "Scene.h"

#include <magic_enum.hpp>

#include "Core/Configs/RuntimeConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"
#include "Core/Time/Time.h"
#include "Engine/Entities/Components/Camera.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/GameObjects/SceneViewCamera.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"

// TODO: TESTING
#include "Engine/Entities/Components/Visualizer.h"
#include "Engine/Entities/Components/Colliders/BoxCollider.h"
#include "Engine/Entities/Components/Colliders/SphereCollider.h"
#include "Engine/Entities/Components/PhysicsBody.h"
#include <SimpleMath.h>
#include <fstream>
using namespace DirectX::SimpleMath;

using namespace Flux;

Scene::Scene() : sceneName("DefaultScene")
{
	SceneContext::SetScene(this);

	// INFO: Setup Events to Listen For
	EventDispatcher::AddListener(EventType::GameObjectRemoved, this);
	EventDispatcher::AddListener(EventType::PlayModeExited, this);
	EventDispatcher::AddListener(EventType::SceneSaved, this);

	// INFO: Create a scene view camera
	sceneViewCamera = std::make_unique<SceneViewCamera>();

	// INFO: Setup the physics scene
	auto& physics = Physics::GetPhysics();

	physx::PxSceneDesc sceneDesc(physics.getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = Physics::CustomFilterShader;
	sceneDesc.simulationEventCallback = this;

	// INFO: Create the PhysX Scene
	physicsScene = physics.createScene(sceneDesc);

	if (!physicsScene)
	{
		Debug::LogError("Scene::Scene() - Failed to create PhysX Scene");
	}

	// INFO: Create a default play mode camera
	//gameObjects.emplace_back(std::make_unique<GameObject>());
	//playModeCamera = gameObjects.back().get()->AddComponent<Camera>(gameObjects.back().get());

	// TODO: Testing
	/*gameObjects.emplace_back(std::make_unique<GameObject>());
	gameObjects.back().get()->AddComponent<Visualizer>(gameObjects.back().get());
	gameObjects.back().get()->AddComponent<BoxCollider>(gameObjects.back().get());
	auto visualizer = gameObjects.back().get()->GetComponent<Visualizer>().lock();
	auto transform = gameObjects.back().get()->GetComponent<Transform>().lock();
	transform->SetPosition(Vector3(0.0f, 0.0f, 5.0f));
	transform->SetRotation(Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, DirectX::XMConvertToRadians(40.0f)));
	gameObjects.back().get()->AddComponent<PhysicsBody>(gameObjects.back().get());

	gameObjects.emplace_back(std::make_unique<GameObject>());
	gameObjects.back().get()->AddComponent<Visualizer>(gameObjects.back().get());
	gameObjects.back().get()->GetComponent<Visualizer>().lock()->SetModel("Sphere");
	gameObjects.back().get()->AddComponent<SphereCollider>(gameObjects.back().get());
	gameObjects.back().get()->GetComponent<SphereCollider>().lock()->SetRadius(3.0f);
	transform = gameObjects.back().get()->GetComponent<Transform>().lock();
	transform->SetPosition(Vector3(-10.0f, 0.0f, 5.0f));

	gameObjects.emplace_back(std::make_unique<GameObject>());
	gameObjects.back().get()->AddComponent<Visualizer>(gameObjects.back().get());
	gameObjects.back().get()->AddComponent<BoxCollider>(gameObjects.back().get());
	transform = gameObjects.back().get()->GetComponent<Transform>().lock();
	transform->SetPosition(Vector3(0.0f, -10.0f, 0.0f));
	transform->SetScale(Vector3(10.0f, 1.0f, 10.0f));
	auto boxCollider = gameObjects.back().get()->GetComponent<BoxCollider>().lock();
	boxCollider->SetSize(10.0f, 1.0f, 10.0f);*/

	// TODO: Serialization Testing
	//nlohmann::ordered_json json;
	//Serialize(json);
	//std::ofstream jsonTest("test.json");
	//jsonTest << json.dump(4);

	// TODO: Deserialization Testing
	std::ifstream jsonTest("test.json");
	nlohmann::ordered_json json = nlohmann::ordered_json::parse(jsonTest);
	Deserialize(json);
}

Scene::~Scene()
{
	if (physicsScene)
	{
		physicsScene->release();
		physicsScene = nullptr;
	}
}

void Scene::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Save the name of the Scene
	json["SceneName"] = sceneName; // TODO: For now only one scene is supported

	// INFO: Create a JSON array to store all GameObjects
	json["GameObjects"] = nlohmann::json::array();

	// INFO: Serialize each GameObject in the Scene
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->Serialize(json);
	}
}

void Scene::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Clear existing scene contents before loading 'new' scene
	gameObjects.clear();
	GameObject::ClearGameObjectTypeCounters();
	components.clear();
	debugWireframes.clear();
	rigidActorsToColliders.clear();
	playModeCamera.reset();

	// INFO: Load the name of the Scene
	sceneName = json["SceneName"].get<std::string>();

	// INFO: Find out how many game objects are in the scene
	size_t gameObjectCount = json["GameObjects"].size();

	// INFO: Deserialize each GameObject in the Scene
	for (size_t i = 0; i < gameObjectCount; i++)
	{
		// INFO: Retrieve the game object data from the json file
		auto& gameObjectData = json["GameObjects"][i];

		// INFO: Create the GameObject based on the type
		gameObjects.emplace_back(GameObject::CreateGameObject(gameObjectData["Type"].get<std::string>()));

		// INFO: Deserialize the newly created GameObject with the gameObjects data
		gameObjects.back()->Deserialize(gameObjectData);
	}
}

void Scene::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
	if (eventType == EventType::GameObjectRemoved)
	{
		auto gameObjectRemovedEvent = std::static_pointer_cast<GameObjectRemovedEvent>(event);

		// INFO: Remove the GameObject from the Scene
		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			GameObject* gameObject = gameObjects[i].get();

			if (gameObject == gameObjectRemovedEvent->gameObject)
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

		// INFO: Remove all now-expired rigid actors associated with the removed GameObject
		for (auto it = rigidActorsToColliders.begin(); it != rigidActorsToColliders.end();)
		{
			if (it->second.expired())
			{
				it = rigidActorsToColliders.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
	else if (eventType == EventType::PlayModeExited)
	{
		// TODO: TESTING CODE
		std::ifstream jsonTest("test.json");
		nlohmann::ordered_json json = nlohmann::ordered_json::parse(jsonTest);
		Deserialize(json);
	}
	else if (eventType == EventType::SceneSaved)
	{
		// TODO: TESTING CODE
		nlohmann::ordered_json json;
		Serialize(json);
		std::ofstream jsonFile("test.json");
		jsonFile << json.dump(4);
	}
}

void Scene::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	physx::PxRigidActor* actor0 = pairHeader.actors[0];
	physx::PxRigidActor* actor1 = pairHeader.actors[1];

	std::shared_ptr<Collider> collider0 = rigidActorsToColliders[actor0].lock();
	std::shared_ptr<Collider> collider1 = rigidActorsToColliders[actor1].lock();

	if (!collider0 || !collider1)
	{
		Debug::LogError("Scene::onContact() - Collider is expired");
		return;
	}

	for (physx::PxU32 i = 0; i < nbPairs; i++)
	{
		// INFO: OnCollisionEnter
		if (pairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			collider0->ExecuteCollisionCallback(CollisionType::CollisionEnter, collider1);
			collider1->ExecuteCollisionCallback(CollisionType::CollisionEnter, collider0);
		}
		// INFO: OnCollisionExit
		else if (pairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			collider0->ExecuteCollisionCallback(CollisionType::CollisionExit, collider1);
			collider1->ExecuteCollisionCallback(CollisionType::CollisionExit, collider0);
		}
	}
}

void Scene::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	physx::PxRigidActor* triggerActor = pairs->triggerActor;
	physx::PxRigidActor* otherActor = pairs->otherActor;

	std::shared_ptr<Collider> triggerCollider = rigidActorsToColliders[triggerActor].lock();
	std::shared_ptr<Collider> otherCollider = rigidActorsToColliders[otherActor].lock();

	if (!triggerCollider || !otherCollider)
	{
		Debug::LogError("Scene::onTrigger() - Other Collider is expired");
		return;
	}

	for (physx::PxU32 i = 0; i < count; i++)
	{
		// INFO: OnTriggerEnter
		if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			triggerCollider->ExecuteCollisionCallback(CollisionType::TriggerEnter, otherCollider);
		}
		// INFO: OnTriggerExit
		else if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			triggerCollider->ExecuteCollisionCallback(CollisionType::TriggerExit, otherCollider);
		}
	}
}

void Scene::Start()
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		std::unique_ptr<GameObject>& gameObject = gameObjects[i];

		if (!gameObject->IsActive()) { continue; }

		gameObject->Start();
	}
}

void Scene::Update(float deltaTime)
{
	// INFO: Update all custom user scripts
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		std::unique_ptr<GameObject>& gameObject = gameObjects[i];

		if (!gameObject->IsActive()) { continue; }

		gameObject->Update(deltaTime);
	}

	// INFO: Update Colliders (Physics Simulation if PhysicsBody, otherwise updated using Transform)
	std::vector<std::weak_ptr<Collider>> colliders;

	auto boxColliders = GetComponents<BoxCollider>();
	for (auto& boxCollider : boxColliders) { colliders.push_back(boxCollider); } 

	auto sphereColliders = GetComponents<SphereCollider>();
	for (auto& sphereCollider : sphereColliders) { colliders.push_back(sphereCollider); }

	for (size_t i = 0; i < colliders.size(); i++)
	{
		std::shared_ptr<Collider> collider = colliders[i].lock();

		if (!collider || !collider->GetGameObject()->IsActive() || !collider->IsActive()) { continue; }

		collider->Update(Time::Alpha());
	}
}

void Scene::LateUpdate(float deltaTime)
{
	if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
	{
		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			std::unique_ptr<GameObject>& gameObject = gameObjects[i];

			if (!gameObject->IsActive()) { continue; }

			gameObject->LateUpdate(deltaTime);
		}
	}

	if (RuntimeConfig::IsInEditorMode() || RuntimeConfig::IsPaused()) { sceneViewCamera->LateUpdate(deltaTime); }
}

void Scene::FixedUpdate(float fixedDeltaTime)
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		std::unique_ptr<GameObject>& gameObject = gameObjects[i];

		if (!gameObject->IsActive()) { continue; }

		gameObject->FixedUpdate(fixedDeltaTime);
	}
}

void Scene::DrawWireframes(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
{
	for (size_t i = 0; i < debugWireframes.size(); i++)
	{
		DebugWireframeData& debugWireframeData = debugWireframes[i];
		std::shared_ptr<Component> component = debugWireframeData.component.lock();

		if (!component || !component->GetGameObject()->IsActive() || !component->IsActive()) { continue; }

		debugWireframeData.debugWireframe->DrawWireframe(deviceContext, primitiveBatch);
	}
}

void Scene::RegisterComponent(std::weak_ptr<Component> component)
{
	if (component.expired())
	{
		Debug::LogError("Scene::RegisterComponent() - Component is expired");
		return;
	}

	std::shared_ptr<Component> validComponent = component.lock();

	switch (validComponent->GetComponentType())
	{
	// INFO: Both inherit from IDebugWireframe
	case ComponentType::BoxCollider:
	case ComponentType::SphereCollider:
	{
		std::weak_ptr<Collider> collider = std::dynamic_pointer_cast<Collider>(validComponent);

		if (std::shared_ptr<Collider> validCollider = collider.lock())
			RegisterRigidActorToCollider(collider, validCollider->GetRigidActor());

	}
		[[fallthrough]];
	case ComponentType::Camera:
	{
		std::weak_ptr<IDebugWireframe> debugWireframe = std::dynamic_pointer_cast<IDebugWireframe>(validComponent);

		if (!debugWireframe.expired())
		{
			DebugWireframeData debugWireframeData(component, debugWireframe.lock().get());
			debugWireframes.push_back(debugWireframeData);
		}
		else
		{
			Debug::LogError("Scene::RegisterComponent() - Component could not be added to IDebugWireframe container. Component Type: " + 
							std::string(magic_enum::enum_name(validComponent->GetComponentType())));
		}

		break;
	}
	default:
		break;
	}

	components[validComponent->GetComponentType()].push_back(component);
}

void Scene::RemoveRigidActorToColliderEntry(physx::PxRigidActor* rigidActor)
{
	auto it = rigidActorsToColliders.find(rigidActor);
	if (it != rigidActorsToColliders.end()) { rigidActorsToColliders.erase(it); }
}

void Scene::RegisterRigidActorToCollider(std::weak_ptr<Collider> collider, physx::PxRigidActor* rigidActor)
{
	rigidActorsToColliders[rigidActor] = collider;
}

std::weak_ptr<Collider> Scene::GetCollider(physx::PxRigidActor* rigidActor)
{
	auto it = rigidActorsToColliders.find(rigidActor);
	if (it != rigidActorsToColliders.end()) { return it->second; }

	return std::weak_ptr<Collider>();
}

std::weak_ptr<Camera> Scene::GetCamera()
{
	if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
	{
		// INFO: Return the first active camera in the scene
		if (playModeCamera.expired() || !playModeCamera.expired() && !playModeCamera.lock()->IsActive())
		{
			playModeCamera = FindFirstActiveCamera();
		}

		return playModeCamera;
	}
	else
	{
		return sceneViewCamera->GetCamera();
	}
}

std::weak_ptr<Camera> Scene::FindFirstActiveCamera()
{
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		std::unique_ptr<GameObject>& gameObject = gameObjects[i];

		if (gameObject->HasComponent<Camera>())
		{
			std::shared_ptr<Camera> camera = gameObject->GetComponent<Camera>().lock();

			if (!gameObject->IsActive() || !camera->IsActive()) { continue; }

			return camera;
		}
	}

	Debug::LogError("Scene::FindFirstActiveCamera() - No active camera found in the scene, constructing a default one");
	gameObjects.emplace_back(GameObject::CreateGameObject("GameObject"));

	return gameObjects.back().get()->AddComponent<Camera>(gameObjects.back().get());
}
