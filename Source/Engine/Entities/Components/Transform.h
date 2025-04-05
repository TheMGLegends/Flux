#pragma once

#include "Component.h"

#include <SimpleMath.h>

namespace Flux
{
	class Transform : public Component
	{
	public:
		Transform(GameObject* _gameObject);
		virtual ~Transform() override;

		virtual void DrawDetails() override;

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		DirectX::XMMATRIX GetWorldMatrix() const;
		DirectX::XMMATRIX GetWorldMatrix(const DirectX::SimpleMath::Vector3& otherScale) const;

		DirectX::SimpleMath::Vector3 Forward() const;
		DirectX::SimpleMath::Vector3 Right() const;
		DirectX::SimpleMath::Vector3 Up() const;

		inline void SetPosition(const DirectX::SimpleMath::Vector3& _position) { position = _position; }
		inline const DirectX::SimpleMath::Vector3& GetPosition() const { return position; }

		inline void SetRotation(const DirectX::SimpleMath::Quaternion& _rotation) { rotation = _rotation; }
		inline const DirectX::SimpleMath::Quaternion& GetRotation() const { return rotation; }

		inline void SetScale(const DirectX::SimpleMath::Vector3& _scale) { scale = _scale; }
		inline const DirectX::SimpleMath::Vector3& GetScale() const { return scale; }

		void Rotate(const DirectX::SimpleMath::Vector3& eulerAngles);
		inline void Translate(const DirectX::SimpleMath::Vector3& translation) { position += translation; }

		/// @brief Used internally by the Editor to set the position of the Transform and any associated rigidActors
		void SetPositionEditor(const DirectX::SimpleMath::Vector3& _position);

		/// @brief Used internally by the Editor to set the rotation of the Transform and any associated rigidActors
		void SetRotationEditor(const DirectX::SimpleMath::Quaternion& _rotation);

	private:
		void DisplayVector3Field(const char* label, DirectX::SimpleMath::Vector3& value, float speed = 0.1f);

	private:
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 scale;
	};
}

