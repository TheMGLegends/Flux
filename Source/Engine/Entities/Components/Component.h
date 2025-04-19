#pragma once

#include "Engine/Interfaces/ISerializable.h"

#include "ComponentTypes.h"

namespace DirectX::SimpleMath { struct Vector3; }

namespace Flux
{
	class GameObject;

	class Component : public ISerializable
	{
		friend class GameObject;

	public:
		explicit Component(GameObject* _gameObject);

		/// @brief Called after the Component is constructed and added to the GameObject
		virtual void PostConstruction() { /*INFO: Default Implementation Left Blank*/ }

		/// @brief Called after the component is removed from the GameObject
		virtual void PostDestruction() { /*INFO: Default Implementation Left Blank*/ }

		/// @brief Called before GameObject start inside of Scene start
		virtual void Start() { /*INFO: Default Implementation Left Blank*/ }

		/// @brief Called by details panel, should implement logic to draw the component details using ImGui
		virtual void DrawDetails() { /*INFO: Default Implementation Left Blank*/ }

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		GameObject* GetGameObject() const;

		virtual void SetIsActive(bool _isActive);
		bool IsActive() const;

		bool CanHaveMultiple() const;
		bool IsRemoveable() const;

		ComponentType GetComponentType() const;

	protected:
		/// @min Applies to the X value
		/// @max Applies to the X value
		/// @restrictAll If true, all three values will be clamped to the min and max otherwise only X
		/// @return true if the field was changed
		bool DisplayVector3Field(const char* label, DirectX::SimpleMath::Vector3& value, float speed = 0.1f, 
								 const char* format = "%.2f", float min = 0.0f, float max = 0.0f, bool restrictAll = false);

	private:
		void SetGameObject(GameObject* _gameObject);

	protected:
		bool isActive;
		bool canHaveMultiple;
		bool isRemoveable;

		std::string name;
		ComponentType componentType;

	private:
		GameObject* gameObject;

		bool draggingBox;
	};
}

