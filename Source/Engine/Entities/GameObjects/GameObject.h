#pragma once

#include "Engine/Interfaces/ISerializable.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Helpers/StringHasher.h"

#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/Components/Colliders/Collider.h"

namespace Flux
{
	class Component;

	class GameObject : public ISerializable
	{
	public:
		GameObject();
		~GameObject() override;

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		template<class T>
		bool HasComponent();

		template<class T>
		std::weak_ptr<T> GetComponent();

		template<class T, typename... Args>
		std::weak_ptr<T> AddComponent(Args&&... args);

		template<class T>
		bool RemoveComponent(std::weak_ptr<T> component);

		void SetIsActive(bool _isActive);
		bool IsActive() const { return isActive; }

		std::vector<std::shared_ptr<Component>>& GetComponents() { return components; }

		void Destroy();

		virtual void Start() { /*INFO: Default Implementation Left Blank*/ }
		virtual void Update(float deltaTime) { /*INFO: Default Implementation Left Blank*/ }
		virtual void LateUpdate(float deltaTime) { /*INFO: Default Implementation Left Blank*/ }
		virtual void FixedUpdate(float fixedDeltaTime) { /*INFO: Default Implementation Left Blank*/ }

		virtual void OnCollisionEnter(std::shared_ptr<Collider> other) { /*INFO: Default Implementation Left Blank*/ }
		virtual void OnCollisionExit(std::shared_ptr<Collider> other) { /*INFO: Default Implementation Left Blank*/ }

		virtual void OnTriggerEnter(std::shared_ptr<Collider> other) { /*INFO: Default Implementation Left Blank*/ }
		virtual void OnTriggerExit(std::shared_ptr<Collider> other) { /*INFO: Default Implementation Left Blank*/ }

		virtual void OnDisable() { /*INFO: Default Implementation Left Blank*/ }
		virtual void OnEnable() { /*INFO: Default Implementation Left Blank*/ }
		virtual void OnDestroy() { /*INFO: Default Implementation Left Blank*/ }

		/// @brief Used to set display name of GameObject
		void SetName(std::string_view _name);
		std::string& GetName();

	private:
		void SetType(std::string_view _type);

	public:
		std::weak_ptr<Transform> transform;

	private:
		bool isActive;
		std::vector<std::shared_ptr<Component>> components;

		std::string name; // INFO: Used by Editor GUI to display the name of the GameObject
		std::string type; // INFO: Used by Deserialization to determine the type of the GameObject to instantiate

	// INFO: GameObject Factory (Reflection)
	public:
		static std::unique_ptr<GameObject> CreateGameObject(const std::string& typeName);
		static const std::unordered_map<std::string, std::function<std::unique_ptr<GameObject>()>, StringHasher, std::equal_to<>>& GetGameObjectTypes();
		static void ClearGameObjectTypeCounters() { gameObjectCounter = -1; gameObjectTypeCounters.clear(); }

	protected:
		static void RegisterGameObjectType(const std::string& typeName, std::function<std::unique_ptr<GameObject>()> creator);

#define REFLECT(className) \
	static std::unique_ptr<GameObject> Create() { return std::make_unique<className>(); } \
	struct ObjectRegister \
	{ \
		ObjectRegister() { GameObject::RegisterGameObjectType(#className, &className::Create); } \
	}; \
	inline static ObjectRegister objectRegister;

	private:
		static inline std::unordered_map<std::string, std::function<std::unique_ptr<GameObject>()>, StringHasher, std::equal_to<>> gameObjectTypes;
		static inline std::unordered_map<std::string, int, StringHasher, std::equal_to<>> gameObjectTypeCounters;
		static int gameObjectCounter; // INFO: Only for Game Objects since they don't get reflected
	};

	template<class T>
	inline bool GameObject::HasComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

		// INFO: Check if component exists
		for (auto& component : components)
		{
			std::weak_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(component);

			if (!castedComponent.expired()) { return true; }
		}
		return false;
	}

	template<class T>
	inline std::weak_ptr<T> GameObject::GetComponent()
	{
		static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

		// INFO: Check if component exists
		for (auto& component : components)
		{
			std::weak_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(component);

			if (!castedComponent.expired()) { return castedComponent; }
		}

		return std::weak_ptr<T>();
	}

	template<class T, typename ...Args>
	inline std::weak_ptr<T> GameObject::AddComponent(Args && ...args)
	{
		static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

		// INFO: Do not add the component if it already exists and we can only have one
		if (std::weak_ptr<T> existingComponent = GetComponent<T>(); !existingComponent.expired() && !existingComponent.lock()->CanHaveMultiple()) 
		{ 
			return existingComponent; 
		}

		// INFO: Special Case for Collider Components
		if (std::is_base_of_v<Collider, T>)
		{
			std::shared_ptr<Collider> existingCollider = GetComponent<Collider>().lock();

			if (existingCollider)
			{
				std::shared_ptr<T> castedCollider = std::dynamic_pointer_cast<T>(existingCollider);

				if (castedCollider)
				{
					return castedCollider;
				}
				else
				{
					return std::weak_ptr<T>{}; // INFO: Workaround when contains box but sphere collider is added and vice versa
				}
			}
		}

		components.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
		std::weak_ptr<T> newComponent = std::dynamic_pointer_cast<T>(components.back());
		components.back()->PostConstruction();

		SceneContext::GetScene().RegisterComponent(newComponent);

		return newComponent;
	}

	template<class T>
	inline bool GameObject::RemoveComponent(std::weak_ptr<T> component)
	{
		static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

		std::shared_ptr<T> validComponent = component.lock();

		if (!validComponent || !validComponent->IsRemoveable()) { return false; }

		for (auto it = components.begin(); it != components.end();)
		{
			std::shared_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(*it);

			if (castedComponent && castedComponent == validComponent)
			{
				components.erase(it);
				validComponent->PostDestruction();
				EventDispatcher::QueueEvent(EventType::ComponentRemoved, nullptr);
				return true;
			}
			else
			{
				++it;
			}
		}

		return false;
	}
}
