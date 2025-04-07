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

		virtual void Serialize(nlohmann::flux_json& json) const override;
		virtual void Deserialize(const nlohmann::flux_json& json) override;

		DirectX::XMMATRIX GetWorldMatrix() const;
		DirectX::XMMATRIX GetWorldMatrix(const DirectX::SimpleMath::Vector3& offsetScale) const;

		DirectX::SimpleMath::Vector3 Forward() const;
		DirectX::SimpleMath::Vector3 Right() const;
		DirectX::SimpleMath::Vector3 Up() const;

		inline void SetPosition(const DirectX::SimpleMath::Vector3& _position) { position = _position; }
		inline const DirectX::SimpleMath::Vector3& GetPosition() const { return position; }

		inline void SetRotation(const DirectX::SimpleMath::Quaternion& _rotation) { rotation = _rotation; }
		inline const DirectX::SimpleMath::Quaternion& GetRotation() const { return rotation; }
		
		void SetScale(const DirectX::SimpleMath::Vector3& _scale); // TODO: NEEDS REDOING IN COMPONENT START FUNCTION MAYBE
		inline const DirectX::SimpleMath::Vector3& GetScale() const { return scale; }

		void Rotate(const DirectX::SimpleMath::Vector3& eulerAngles);
		inline void Translate(const DirectX::SimpleMath::Vector3& translation) { position += translation; }

		/// @brief Used internally by the Editor to set the position of the Transform and any associated rigidActors 
		void SetPositionEditor(const DirectX::SimpleMath::Vector3& _position); // TODO: NEEDS REDOING IN COMPONENT START FUNCTION

		/// @brief Used internally by the Editor to set the rotation of the Transform and any associated rigidActors
		void SetRotationEditor(const DirectX::SimpleMath::Quaternion& _rotation); // TODO: NEEDS REDOING IN COMPONENT START FUNCTION

	private:
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 scale;
	};
}

