#pragma once

#include "Collider.h"

namespace Flux
{
	class BoxCollider : public Collider, public std::enable_shared_from_this<BoxCollider>
	{
	public:
		explicit BoxCollider(GameObject* _gameObject);
		~BoxCollider() override;

		void Start() override;
		void DrawDetails() override;

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		void DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch) override;

		void SetColliderShape() override;
		void UpdateScale() override;

		void SetSize(const DirectX::SimpleMath::Vector3& _size);
		void SetSize(float xHalfExtent, float yHalfExtent, float zHalfExtent);
		const DirectX::SimpleMath::Vector3& GetSize() const;

	private:
		DirectX::SimpleMath::Vector3 size;
	};
}

