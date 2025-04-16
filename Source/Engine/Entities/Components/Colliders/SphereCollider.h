#pragma once

#include "Collider.h"

namespace Flux
{
	class SphereCollider : public Collider, public std::enable_shared_from_this<SphereCollider>
	{
	public:
		explicit SphereCollider(GameObject* _gameObject);
		~SphereCollider() override;

		void Start() override;
		void DrawDetails() override;

		void Serialize(nlohmann::flux_json& json) const override;
		void Deserialize(const nlohmann::flux_json& json) override;

		void DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch) override;
		
		void SetColliderShape() override;
		void UpdateScale() override;

		void SetRadius(float _radius);
		float GetRadius() const;

	private:
		void DrawRing(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch, const DirectX::XMVECTOR& centre,
					  const DirectX::XMVECTOR& majorAxis, const DirectX::XMVECTOR& minorAxis, bool isTrigger) const;

	private:
		float radius;
	};
}

