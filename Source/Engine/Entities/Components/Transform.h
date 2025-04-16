#pragma once

#include "Component.h"

#include <SimpleMath.h>

namespace Flux
{
	class Transform : public Component
	{
	public:
		explicit Transform(GameObject* _gameObject);
		~Transform() override;

		void DrawDetails() override;

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		DirectX::XMMATRIX GetWorldMatrix() const;
		DirectX::XMMATRIX GetWorldMatrix(const DirectX::SimpleMath::Vector3& offsetScale) const;

		DirectX::SimpleMath::Vector3 Forward() const;
		DirectX::SimpleMath::Vector3 Right() const;
		DirectX::SimpleMath::Vector3 Up() const;

		void SetPosition(const DirectX::SimpleMath::Vector3& _position);
		const DirectX::SimpleMath::Vector3& GetPosition() const;

		void SetRotation(const DirectX::SimpleMath::Quaternion& _rotation);
		const DirectX::SimpleMath::Quaternion& GetRotation() const;
		
		void SetScale(const DirectX::SimpleMath::Vector3& _scale);
		const DirectX::SimpleMath::Vector3& GetScale() const;

		void Rotate(const DirectX::SimpleMath::Vector3& eulerAngles);
		void Translate(const DirectX::SimpleMath::Vector3& translation);

	private:
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Quaternion rotation;
		DirectX::SimpleMath::Vector3 scale;
	};
}

