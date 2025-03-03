#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider();
	virtual ~SphereCollider() override;

	virtual void Serialize(nlohmann::ordered_json& json) const override;
	virtual void Deserialize(const nlohmann::ordered_json& json) override;

	virtual void DrawWireframe(ID3D11DeviceContext* deviceContext) override;

	void SetRadius(float _radius);
	inline float GetRadius() const { return radius; }

private:
	float radius;
};

