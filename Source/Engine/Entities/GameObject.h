#pragma once

#include "Engine/Interfaces/ISerializable.h"

#include <memory>
#include <string>
#include <vector>

#include "Components/Colliders/Collider.h"
#include "Components/Transform.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/ComponentRemovedEvent.h"

namespace Flux
{
	class Component;

	class GameObject : public ISerializable
	{
	public:
		GameObject();
		virtual ~GameObject();

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		template<class T>
		std::weak_ptr<T> GetComponent();

		template<class T, typename... Args>
		std::weak_ptr<T> AddComponent(Args&&... args);

		template<class T>
		void RemoveComponent(std::weak_ptr<T> component);

		void SetIsActive(bool _isActive);
		inline bool IsActive() const { return isActive; }

		void Destroy();

		virtual void Start() {}
		virtual void Update(float deltaTime) {}
		virtual void LateUpdate(float deltaTime) {}
		virtual void FixedUpdate(float fixedDeltaTime) {}

		virtual void OnCollisionEnter(std::shared_ptr<Collider> other) {}
		virtual void OnCollisionStay(std::shared_ptr<Collider> other) {}
		virtual void OnCollisionExit(std::shared_ptr<Collider> other) {}

		virtual void OnTriggerEnter(std::shared_ptr<Collider> other) {}
		virtual void OnTriggerStay(std::shared_ptr<Collider> other) {}
		virtual void OnTriggerExit(std::shared_ptr<Collider> other) {}

		virtual void OnDisable() {}
		virtual void OnEnable() {}
		virtual void OnDestroy() {}

	private:
		inline void SetName(const std::string& _name) { name = _name; } // INFO: Used by Editor GUI to set the display name of the GameObject
		inline void SetType(const std::string& _type) { type = _type; } // INFO: Used during Deserialization to note down GO type for future Serialization

	public:
		std::weak_ptr<Transform> transform;

	private:
		bool isActive;
		std::vector<std::shared_ptr<Component>> components;

		std::string name; // INFO: Used by Editor GUI to display the name of the GameObject
		std::string type; // INFO: Used by Serialization to determine the type of the GameObject to instantiate
	};

	template<class T>
	inline std::weak_ptr<T> GameObject::GetComponent()
	{
		// INFO: Check if component exists
		for (auto& component : components)
		{
			std::weak_ptr<T> castedComponent = std::dynamic_pointer_cast<T>(component);

			if (!castedComponent.expired())
				return castedComponent;
		}

		return std::weak_ptr<T>();
	}

	template<class T, typename ...Args>
	inline std::weak_ptr<T> GameObject::AddComponent(Args && ...args)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		std::weak_ptr<T> existingComponent = GetComponent<T>();

		// INFO: Do not add the component if it already exists and we can only have one
		if (!existingComponent.expired() && !existingComponent.lock()->CanHaveMultiple())
			return existingComponent;

		components.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
		std::weak_ptr<T> newComponent = std::dynamic_pointer_cast<T>(components.back());

		// TODO: Add component to the component handler

		return std::dynamic_pointer_cast<T>(components.back());
	}

	template<class T>
	inline void GameObject::RemoveComponent(std::weak_ptr<T> component)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

		if (component.expired() || !component.IsRemovable())
			return;

		for (auto it = components.begin(); it != components.end(); ++it)
		{
			T* castedComponent = dynamic_cast<T*>(it->get());

			if (castedComponent && component.get() == castedComponent)
			{
				EventDispatcher::QueueEvent(EventType::ComponentRemoved, std::make_shared<ComponentRemovedEvent>(component));

				if (!component.expired())
					component.lock()->SetIsActive(false);
			}
		}
	}
}
