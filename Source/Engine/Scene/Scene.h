#pragma once

#include "Engine/Interfaces/ISerializable.h"
#include "Core/EventSystem/IEventListener.h"
#include <PxSimulationEventCallback.h>

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

	public:
		Scene();
		~Scene();

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;

		virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {}
		virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override {}
		virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override {}
		virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {}

		void Start();
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);

		void DrawWireframes(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch);

		void RegisterComponent(std::weak_ptr<Component> component);

		void RemoveRigidActorToColliderEntry(physx::PxRigidActor* rigidActor);
		void RegisterRigidActorToCollider(std::weak_ptr<Collider> collider, physx::PxRigidActor* rigidActor);
		std::weak_ptr<Collider> GetCollider(physx::PxRigidActor* rigidActor);

		template<class T>
		std::vector<std::weak_ptr<T>> GetComponents();

		/// @brief Returns the scene view camera if in editor mode otherwise returns play mode camera
		std::weak_ptr<Camera> GetCamera() const;

		inline physx::PxScene& GetPhysicsScene() const { return *physicsScene; }

	private:
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

		physx::PxScene* physicsScene;
	};

	template<class T>
	inline std::vector<std::weak_ptr<T>> Scene::GetComponents()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		std::vector<std::weak_ptr<T>> componentList;
		ComponentType componentType = ComponentType::None;

		if constexpr (std::is_same<T, Transform>::value)
			componentType = ComponentType::Transform;
		else if constexpr (std::is_same<T, Camera>::value)
			componentType = ComponentType::Camera;
		else if constexpr (std::is_same<T, PhysicsBody>::value)
			componentType = ComponentType::PhysicsBody;
		else if constexpr (std::is_same<T, Visualizer>::value)
			componentType = ComponentType::Visualizer;
		else if constexpr (std::is_same<T, BoxCollider>::value)
			componentType = ComponentType::BoxCollider;
		else if constexpr (std::is_same<T, SphereCollider>::value)
			componentType = ComponentType::SphereCollider;

		for (size_t i = 0; i < components[componentType].size(); i++)
		{
			if (components[componentType][i].expired())
				continue;

			componentList.push_back(std::static_pointer_cast<T>(components[componentType][i].lock()));
		}

		return componentList;
	}
}

