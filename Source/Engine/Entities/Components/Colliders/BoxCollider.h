#pragma once

#include "Collider.h"

namespace Flux
{
	class BoxCollider : public Collider
	{
	public:
		BoxCollider(GameObject* _gameObject);
		virtual ~BoxCollider() override;

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		virtual void DrawWireframe(ID3D11DeviceContext& deviceContext) override;

		void SetSize(const DirectX::SimpleMath::Vector3& _size);
		inline const DirectX::SimpleMath::Vector3& GetSize() const { return size; }

	private:
		DirectX::SimpleMath::Vector3 size;
	};
}

