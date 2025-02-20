#include "BoxCollider.h"

using namespace DirectX::SimpleMath;

BoxCollider::BoxCollider() : size(Vector3::One)
{
	// TODO: Set colliderShape to BoxShape
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	ColliderComponent::Serialize(json);

	// TODO: Serialize BoxCollider
}

void BoxCollider::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	ColliderComponent::Deserialize(json);

	// TODO: Deserialize BoxCollider
}

void BoxCollider::DrawWireframe(ID3D11DeviceContext* deviceContext)
{
	// TODO: Debug Draw Box Collider Wireframe
}

void BoxCollider::SetSize(const DirectX::SimpleMath::Vector3& _size)
{
	size = _size;

	// TODO: Access colliderShape and update size
}
