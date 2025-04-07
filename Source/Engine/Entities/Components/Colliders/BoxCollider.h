#pragma once

#include "Collider.h"

namespace Flux
{
	class BoxCollider : public Collider, public std::enable_shared_from_this<BoxCollider>
	{
	public:
		BoxCollider(GameObject* _gameObject);
		virtual ~BoxCollider() override;

		virtual void DrawDetails() override;

		virtual void Serialize(nlohmann::flux_json& json) const override;
		virtual void Deserialize(const nlohmann::flux_json& json) override;

		virtual void DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch) override;

		virtual void SetColliderShape() override;
		virtual void UpdateScale() override;

		void SetSize(const DirectX::SimpleMath::Vector3& _size);
		inline void SetSize(float xHalfExtent, float yHalfExtent, float zHalfExtent) { SetSize(DirectX::SimpleMath::Vector3(xHalfExtent, yHalfExtent, zHalfExtent)); }
		inline const DirectX::SimpleMath::Vector3& GetSize() const { return size; }

	private:
		DirectX::SimpleMath::Vector3 size;
	};
}

