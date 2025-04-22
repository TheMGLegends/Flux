#pragma once

#include "Engine/Interfaces/ISerializable.h"
#include "Core/EventSystem/IEventListener.h"
#include <PxSimulationEventCallback.h>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Engine/Entities/Components/Components.h"
#include "Engine/Entities/Components/ComponentTypes.h"

namespace physx 
{ 
	class PxRigidActor;
	class PxScene; 
}

namespace Flux
{
	class GameObject;
	class SceneViewCamera;

	class Scene : public ISerializable, public IEventListener, public physx::PxSimulationEventCallback
	{
		friend class SceneContext;
		friend class SceneHierarchy;

	public:
		Scene();
		~Scene() override;

		int Initialise();

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
		void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

		void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override { /*INFO: Default Implementation Left Blank*/ }
		void onWake(physx::PxActor** actors, physx::PxU32 count) override { /*INFO: Default Implementation Left Blank*/ }
		void onSleep(physx::PxActor** actors, physx::PxU32 count) override { /*INFO: Default Implementation Left Blank*/ }
		void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override { /*INFO: Default Implementation Left Blank*/ }

		void Start() const;
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void FixedUpdate(float fixedDeltaTime) const;

		void DrawWireframes(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch);

		void RegisterComponent(std::weak_ptr<Component> component);

		void RemoveRigidActorToColliderEntry(physx::PxRigidActor* rigidActor);
		void RegisterRigidActorToCollider(std::weak_ptr<Collider> collider, physx::PxRigidActor* rigidActor);
		std::weak_ptr<Collider> GetCollider(physx::PxRigidActor* rigidActor);

		template<class T>
		std::vector<std::weak_ptr<T>> GetComponents();

		/// @brief Returns the scene view camera if in editor mode otherwise returns first active camera in the scene
		/// @param isPrimary If true, returns the first active play mode camera in the scene
		std::weak_ptr<Camera> GetCamera(bool isPrimary = false);

		/// @brief SceneViewCamera (Editor Camera), used for de/serialization of scene view contents
		SceneViewCamera* GetEditorCamera() const;

		const std::string& GetSceneName() const;
		physx::PxScene& GetPhysicsScene() const;

		void CreateDefaultScene(const std::filesystem::path& path);
		void SerializeScene(const std::filesystem::path& path);
		void DeserializeScene(const std::filesystem::path& path);

	private:
		std::weak_ptr<Camera> FindFirstActiveCamera();

		struct DebugWireframeData
		{
			std::weak_ptr<Component> component;
			IDebugWireframe* debugWireframe;

			DebugWireframeData(std::weak_ptr<Component> _component, IDebugWireframe* _debugWireframe) : component(_component), debugWireframe(_debugWireframe) {}
			~DebugWireframeData() = default;
		};

	private:
		std::vector<std::unique_ptr<GameObject>> gameObjects;
		std::unordered_map<ComponentType, std::vector<std::weak_ptr<Component>>> components;
		std::unordered_map<physx::PxRigidActor*, std::weak_ptr<Collider>> rigidActorsToColliders;
		std::vector<DebugWireframeData> debugWireframes;

		std::weak_ptr<Camera> playModeCamera;
		std::unique_ptr<SceneViewCamera> sceneViewCamera;

		std::string sceneName;
		std::filesystem::path scenePath;

		physx::PxScene* physicsScene;
	};

	template<class T>
	inline std::vector<std::weak_ptr<T>> Scene::GetComponents()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

		std::vector<std::weak_ptr<T>> componentList;
		ComponentType componentType = ComponentType::None;

		if constexpr (std::is_same_v<T, Transform>)
		{
			componentType = ComponentType::Transform;
		}
		else if constexpr (std::is_same_v<T, Camera>)
		{
			componentType = ComponentType::Camera;
		}
		else if constexpr (std::is_same_v<T, PhysicsBody>)
		{
			componentType = ComponentType::PhysicsBody;
		}
		else if constexpr (std::is_same_v<T, Visualizer>)
		{
			componentType = ComponentType::Visualizer;
		}
		else if constexpr (std::is_same_v<T, BoxCollider>)
		{
			componentType = ComponentType::BoxCollider;
		}
		else if constexpr (std::is_same_v<T, SphereCollider>)
		{
			componentType = ComponentType::SphereCollider;
		}

		if (const auto& it = components.find(componentType); it != components.end())
		{
			for (const auto& component : it->second)
			{
				if (component.expired()) { continue; }
				componentList.push_back(std::static_pointer_cast<T>(component.lock()));
			}
		}

		return componentList;
	}
}

