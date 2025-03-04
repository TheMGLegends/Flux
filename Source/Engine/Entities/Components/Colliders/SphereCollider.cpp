#include "SphereCollider.h"

using namespace Flux;

SphereCollider::SphereCollider(GameObject* _gameObject) : Collider(_gameObject), radius(1.0f)
{
	// TODO: Set colliderShape to SphereShape
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Collider::Serialize(json);

	// TODO: Serialize SphereCollider
}

void SphereCollider::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Collider::Deserialize(json);

	// TODO: Deserialize SphereCollider
}

void SphereCollider::DrawWireframe(ID3D11DeviceContext& deviceContext)
{
	// TODO: Debug Draw Sphere Collider Wireframe
}

void SphereCollider::SetRadius(float _radius)
{
	radius = _radius;

	// TODO: Access colliderShape and update radius
}
