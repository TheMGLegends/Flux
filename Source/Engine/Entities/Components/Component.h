#pragma once

#include "Engine/Interfaces/ISerializable.h"

#include "ComponentTypes.h"

namespace Flux
{
	class GameObject;

	class Component : public ISerializable
	{
		friend class GameObject;

	public:
		Component(GameObject* _gameObject);
		virtual ~Component() = 0;

		/// @brief Called after the Component is constructed and added to the GameObject
		virtual void PostConstruction() {}

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		inline GameObject* GetGameObject() const { return gameObject; }

		inline void SetIsActive(bool _isActive) { isActive = _isActive; }
		inline bool IsActive() const { return isActive; }

		inline bool CanHaveMultiple() const { return canHaveMultiple; }
		inline bool IsRemoveable() const { return isRemoveable; }

		inline ComponentType GetComponentType() const { return componentType; }

	private:
		inline void SetGameObject(GameObject* _gameObject) { gameObject = _gameObject; }

	protected:
		bool isActive;
		bool canHaveMultiple;
		bool isRemoveable;

		std::string name;
		ComponentType componentType;

	private:
		GameObject* gameObject;
	};
}

