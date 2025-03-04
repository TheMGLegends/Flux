#pragma once

#include "Engine/Interfaces/ISerializable.h"
#include "Core/EventSystem/IEventListener.h"

#include <memory>
#include <unordered_map>
#include <vector>

#include "Engine/Entities/Components/ComponentTypes.h"
#include "Engine/Entities/Components/Components.h"

namespace Flux
{
	class GameObject;

	class Scene : public ISerializable, public IEventListener
	{
		friend class SceneContext;

	public:
		Scene();
		~Scene();

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		void Start();
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);

		void AddComponent(std::weak_ptr<Component> component);

		template<class T>
		std::vector<std::weak_ptr<T>> GetComponents();

		inline std::shared_ptr<Camera> GetCamera() const { return camera.expired() ? nullptr : camera.lock(); }

	private:
		std::vector<std::unique_ptr<GameObject>> gameObjects;
		std::unordered_map<ComponentType, std::vector<std::weak_ptr<Component>>> components;

		std::weak_ptr<Camera> camera;
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
		else if constexpr (std::is_same<T, Collider>::value)
			componentType = ComponentType::Collider;

		for (size_t i = 0; i < components[componentType].size(); i++)
		{
			if (components[componentType][i].expired())
				continue;

			componentList.push_back(std::static_pointer_cast<T>(components[componentType][i].lock()));
		}

		return componentList;
	}
}

