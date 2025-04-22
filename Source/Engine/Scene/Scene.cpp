#include "Scene.h"

#include <fstream>
#include <magic_enum.hpp>

#include "Core/GlobalDefines.h"

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/GameConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/CreateSceneEvent.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"
#include "Core/EventSystem/Events/LoadSceneEvent.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Time/Time.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Entities/Components/Camera.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/GameObjects/SceneViewCamera.h"
#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"

namespace Flux
{
	using namespace GlobalDefines;

	Scene::Scene() : sceneName("New Scene"), physicsScene(nullptr)
	{
		SceneContext::SetScene(this);

		// INFO: Setup Events to Listen For
		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::GameObjectRemoved, this)))
		{
			Debug::LogError("Scene::Scene() - Failed to add GameObjectRemoved event listener");
			return;
		}

		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::ComponentRemoved, this)))
		{
			Debug::LogError("Scene::Scene() - Failed to add ComponentRemoved event listener");
			return;
		}

		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::PlayModeExited, this)))
		{
			Debug::LogError("Scene::Scene() - Failed to add PlayModeExited event listener");
			return;
		}

		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::SaveScene, this)))
		{
			Debug::LogError("Scene::Scene() - Failed to add SaveScene event listener");
			return;
		}

		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::LoadScene, this)))
		{
			Debug::LogError("Scene::Scene() - Failed to add LoadScene event listener");
			return;
		}

		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::CreateScene, this)))
		{
			Debug::LogError("Scene::Scene() - Failed to add CreateScene event listener");
			return;
		}

		// INFO: Create a scene view camera
		sceneViewCamera = std::make_unique<SceneViewCamera>();

		// INFO: Setup the physics scene
		physx::PxPhysics& physics = Physics::GetPhysics();

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
	}

	Scene::~Scene()
	{
		if (physicsScene)
		{
			physicsScene->release();
			physicsScene = nullptr;
		}

		EventDispatcher::RemoveListener(this);
	}

	int Scene::Initialise()
	{
		// INFO: Load the starter scene found in the game settings
		const std::filesystem::path& starterScenePath = AssetHandler::GetScenePath(GameConfig::GetStarterSceneName());

		if (starterScenePath.empty())
		{
			Debug::LogError("Scene::Initialise() - Failed to load starter scene");
			return FLUX_FAILURE;
		}

		DeserializeScene(starterScenePath);

		return FLUX_SUCCESS;
	}

	void Scene::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Save the name of the Scene
		json["SceneName"] = sceneName;

		// INFO: Create a JSON array to store all GameObjects
		json["GameObjects"] = nlohmann::json::array();

		// INFO: Serialize each GameObject in the Scene
		for (auto& gameObject : gameObjects)
		{
			gameObject->Serialize(json);
		}
	}

	void Scene::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Load the name of the Scene
		if (json.contains("SceneName"))
		{
			sceneName = json["SceneName"].get<std::string>();
		}

		// INFO: Find out how many game objects are in the scene
		if (json.contains("GameObjects"))
		{
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
	}

	void Scene::OnNotify(EventType eventType, std::shared_ptr<Event> event)
	{
		if (eventType == EventType::GameObjectRemoved)
		{
			std::shared_ptr<GameObjectRemovedEvent> gameObjectRemovedEvent = std::static_pointer_cast<GameObjectRemovedEvent>(event);

			// INFO: Remove the GameObject from the Scene
			for (size_t i = 0; i < gameObjects.size(); i++)
			{
				const GameObject* gameObject = gameObjects[i].get();

				if (gameObject == gameObjectRemovedEvent->gameObject)
				{
					gameObjects.erase(gameObjects.begin() + i);
					EditorConfig::SetSceneNeedsSaving(true);
					break;
				}
			}
		}

		if (eventType == EventType::GameObjectRemoved || eventType == EventType::ComponentRemoved)
		{
			// INFO: Remove all expired components
			for (auto& [type, componentList] : components)
			{
				std::erase_if(componentList, [](std::weak_ptr<Component> component)
					{
						return component.expired();
					});
			}

			// INFO: Remove all expired debug wireframes
			std::erase_if(debugWireframes, [](const DebugWireframeData& data)
				{
					return data.component.expired();
				});

			// INFO: Remove all expired rigid actors
			std::erase_if(rigidActorsToColliders, [](const auto& pair)
				{
					return pair.second.expired();
				});
		}
		else if (eventType == EventType::PlayModeExited)
		{
			DeserializeScene(scenePath);
		}
		else if (eventType == EventType::SaveScene)
		{
			SerializeScene(scenePath);

			if (EditorConfig::SceneNeedsSaving()) { EditorConfig::SetSceneNeedsSaving(false); }
		}
		else if (eventType == EventType::LoadScene)
		{
			std::shared_ptr<LoadSceneEvent> loadSceneEvent = std::static_pointer_cast<LoadSceneEvent>(event);

			if (!loadSceneEvent) { return; }

			if (RuntimeConfig::IsInPlayMode())
			{
				// INFO: Unpause the game if it was paused
				if (RuntimeConfig::IsPaused())
				{
					RuntimeConfig::TogglePause();
				}

				Audio::StopAllSounds();

				if (!loadSceneEvent->stayInPlayMode)
				{
					RuntimeConfig::SetMode(RuntimeConfig::Mode::Editor);
				}
				else
				{
					// INFO: Need to run start again on new scene game objects
					RuntimeConfig::SetPlayModeEntered(true);
				}
			}

			// INFO: Clear existing scene contents before loading 'new' scene
			gameObjects.clear();
			GameObject::ClearGameObjectTypeCounters();
			components.clear();
			debugWireframes.clear();
			rigidActorsToColliders.clear();
			playModeCamera.reset();

			DeserializeScene(loadSceneEvent->scenePath);
		}
		else if (eventType == EventType::CreateScene)
		{
			std::shared_ptr<CreateSceneEvent> createSceneEvent = std::static_pointer_cast<CreateSceneEvent>(event);

			CreateDefaultScene(createSceneEvent->scenePath);
			AssetHandler::StoreScenePath(createSceneEvent->sceneName, createSceneEvent->scenePath);
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
			if (!collider0)
			{
				if (rigidActorsToColliders.contains(actor0))
				{
					rigidActorsToColliders.erase(actor0);
				}
			}

			if (!collider1)
			{
				if (rigidActorsToColliders.contains(actor1))
				{
					rigidActorsToColliders.erase(actor1);
				}
			}

			return;
		}

		using enum Flux::CollisionType;

		for (physx::PxU32 i = 0; i < nbPairs; i++)
		{
			// INFO: OnCollisionEnter
			if (pairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				collider0->ExecuteCollisionCallback(CollisionEnter, collider1);
				collider1->ExecuteCollisionCallback(CollisionEnter, collider0);
			}
			// INFO: OnCollisionExit
			else if (pairs[i].events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				collider0->ExecuteCollisionCallback(CollisionExit, collider1);
				collider1->ExecuteCollisionCallback(CollisionExit, collider0);
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
			if (!triggerCollider)
			{
				if (rigidActorsToColliders.contains(triggerActor))
				{
					rigidActorsToColliders.erase(triggerActor);
				}
			}
			if (!otherCollider)
			{
				if (rigidActorsToColliders.contains(otherActor))
				{
					rigidActorsToColliders.erase(otherActor);
				}
			}
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

	void Scene::Start() const
	{
		// INFO: Components Start
		for (const auto& [type, componentList] : components)
		{
			for (const auto& weakComponent : componentList)
			{
				if (std::shared_ptr<Component> component = weakComponent.lock())
				{
					component->Start();
				}
			}
		}

		// INFO: GameObject Start
		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive()) { continue; }
			gameObject->Start();
		}
	}

	void Scene::Update(float deltaTime)
	{
		// INFO: Update all custom user scripts
		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive()) { continue; }
			gameObject->Update(deltaTime);
		}

		// INFO: Update Colliders (Physics Simulation if PhysicsBody, otherwise updated using Transform)
		std::vector<std::weak_ptr<Collider>> colliders;

		auto boxColliders = GetComponents<BoxCollider>();
		for (const auto& boxCollider : boxColliders) { colliders.push_back(boxCollider); }

		auto sphereColliders = GetComponents<SphereCollider>();
		for (const auto& sphereCollider : sphereColliders) { colliders.push_back(sphereCollider); }

		for (const auto& weakCollider : colliders)
		{
			std::shared_ptr<Collider> collider = weakCollider.lock();

			if (!collider || !collider->GetGameObject()->IsActive() || !collider->IsActive()) { continue; }
			collider->Update(Time::Alpha());
		}
	}

	void Scene::LateUpdate(float deltaTime)
	{
		if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
		{
			for (const auto& gameObject : gameObjects)
			{
				if (!gameObject->IsActive()) { continue; }
				gameObject->LateUpdate(deltaTime);
			}
		}

		if (RuntimeConfig::IsInEditorMode() || RuntimeConfig::IsPaused()) { sceneViewCamera->LateUpdate(deltaTime); }
	}

	void Scene::FixedUpdate(float fixedDeltaTime) const
	{
		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive()) { continue; }
			gameObject->FixedUpdate(fixedDeltaTime);
		}
	}

	void Scene::DrawWireframes(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
	{
		for (auto& debugWireframe : debugWireframes)
		{
			if (std::shared_ptr<Component> component = debugWireframe.component.lock(); !component || !component->GetGameObject()->IsActive() || !component->IsActive()) 
			{ 
				continue; 
			}

			debugWireframe.debugWireframe->DrawWireframe(deviceContext, primitiveBatch);
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
			if (std::weak_ptr<IDebugWireframe> debugWireframe = std::dynamic_pointer_cast<IDebugWireframe>(validComponent); !debugWireframe.expired())
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
		if (auto it = rigidActorsToColliders.find(rigidActor); it != rigidActorsToColliders.end()) { return it->second; }

		return std::weak_ptr<Collider>();
	}

	std::weak_ptr<Camera> Scene::GetCamera(bool isPrimary)
	{
		std::shared_ptr<Camera> playCamera = playModeCamera.lock();

		// INFO: Return the first active camera in the scene
		if (isPrimary)
		{
			return FindFirstActiveCamera();
		}

		if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
		{
			return FindFirstActiveCamera();
		}
		else
		{
			return sceneViewCamera->GetCamera();
		}
	}

	SceneViewCamera* Scene::GetEditorCamera() const
	{
		return sceneViewCamera.get();
	}

	const std::string& Scene::GetSceneName() const
	{
		return sceneName;
	}

	physx::PxScene& Scene::GetPhysicsScene() const
	{
		return *physicsScene;
	}

	void Scene::CreateDefaultScene(const std::filesystem::path& path)
	{
		// INFO: Save the current scene
		if (!scenePath.empty())
		{
			Debug::LogWarning("Scene::CreateDefaultScene() - Saving current scene before creating the new one");
			SerializeScene(scenePath);
		}

		// INFO: Clear existing scene contents before loading 'new' scene
		gameObjects.clear();
		GameObject::ClearGameObjectTypeCounters();
		components.clear();
		debugWireframes.clear();
		rigidActorsToColliders.clear();
		playModeCamera.reset();

		// INFO: Create a default play mode camera
		gameObjects.emplace_back(std::make_unique<GameObject>());
		const std::unique_ptr<GameObject>& gameObject = gameObjects.back();
		gameObject->SetName("Main Camera");
		playModeCamera = gameObject->AddComponent<Camera>(gameObject.get());

		// INFO: Create the new scene json file
		SerializeScene(path);
	}

	void Scene::SerializeScene(const std::filesystem::path& path)
	{
		scenePath = path;
		sceneName = scenePath.stem().string();

		std::ofstream jsonFile(scenePath);

		if (jsonFile.is_open())
		{
			nlohmann::flux_json json;
			Serialize(json);
			jsonFile << json.dump(4);
		}
		else
		{
			Debug::LogError("Scene::SerializeScene() - Failed to open scene file for writing. Filepath: " + scenePath.string());
		}
	}

	void Scene::DeserializeScene(const std::filesystem::path& path)
	{
		EditorConfig::SetSceneNeedsSaving(false);

		scenePath = path;

		// INFO: Clear existing scene contents before loading 'new' scene
		gameObjects.clear();
		GameObject::ClearGameObjectTypeCounters();
		components.clear();
		debugWireframes.clear();
		rigidActorsToColliders.clear();
		playModeCamera.reset();

		std::ifstream jsonFile(scenePath);

		if (jsonFile.is_open())
		{
			nlohmann::ordered_json json = nlohmann::ordered_json::parse(jsonFile);
			Deserialize(json);
		}
		else
		{
			Debug::LogError("Scene::DeserializeScene() - Failed to open scene file for reading. Filepath: " + scenePath.string());
		}
	}

	std::weak_ptr<Camera> Scene::FindFirstActiveCamera()
	{
		// INFO: Return if we have a valid camera
		if (std::shared_ptr<Camera> playCamera = playModeCamera.lock(); playCamera && playCamera->GetGameObject()->IsActive() && playCamera->IsActive())
		{
			return playModeCamera;
		}

		for (const auto& gameObject : gameObjects)
		{
			if (!gameObject->IsActive()) { continue; }
			std::shared_ptr<Camera> camera = gameObject->GetComponent<Camera>().lock();
			if (camera && camera->IsActive())
			{
				playModeCamera = camera;
				return playModeCamera;
			}
		}

		if (RuntimeConfig::IsInPlayMode())
		{
			Debug::LogError("Scene::FindFirstActiveCamera() - No active camera found in the scene, constructing a default one");
			gameObjects.emplace_back(GameObject::CreateGameObject("GameObject"));
			playModeCamera = gameObjects.back().get()->AddComponent<Camera>(gameObjects.back().get());
			return playModeCamera;
		}
		else
		{
			return sceneViewCamera->GetCamera();
		}
	}
}
