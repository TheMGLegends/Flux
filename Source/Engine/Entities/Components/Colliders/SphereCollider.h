#pragma once

#include "Collider.h"

namespace Flux
{
	class SphereCollider : public Collider
	{
	public:
		SphereCollider(GameObject* _gameObject);
		virtual ~SphereCollider() override;

		virtual void Serialize(nlohmann::ordered_json& json) const override;
		virtual void Deserialize(const nlohmann::ordered_json& json) override;

		virtual void DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch) override;
		
		virtual void SetColliderShape() override;

		void SetRadius(float _radius);
		inline float GetRadius() const { return radius; }

	private:
		void DrawRing(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch, const DirectX::XMVECTOR& centre,
			const DirectX::XMVECTOR& majorAxis, const DirectX::XMVECTOR& minorAxis, bool isTrigger);

	private:
		float radius;
	};
}

