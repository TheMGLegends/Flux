#include "BoxCollider.h"

#include <DirectXColors.h>

#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/Transform.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

BoxCollider::BoxCollider(GameObject* _gameObject) : Collider(_gameObject), size(Vector3::One)
{
	name = "BoxCollider";
	// TODO: Set colliderShape to BoxShape
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Collider::Serialize(json);

	json["Components"].back()["Size"] = { size.x, size.y, size.z };
}

void BoxCollider::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Collider::Deserialize(json);

	// TODO: Deserialize BoxCollider
}

void BoxCollider::DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
{
	static const DirectX::XMVECTOR vertices[8] =
	{
		{ -1, -1, -1, 0 },
		{ 1, -1, -1, 0 },
		{ 1, -1, 1, 0 },
		{ -1, -1, 1, 0 },
		{ -1, 1, -1, 0 },
		{ 1, 1, -1, 0 },
		{ 1, 1, 1, 0 },
		{ -1, 1, 1, 0 }
	};

	static const unsigned short indices[24] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	std::shared_ptr<Transform> owningTransform = GetGameObject()->transform.lock();

	if (owningTransform)
	{
		DirectX::XMMATRIX world = owningTransform->GetWorldMatrix(size);

		// INFO: Translate the vertices to world space
		DirectX::VertexPositionColor worldVertices[8]{};

		bool isTrigger = IsTrigger();

		for (size_t i = 0; i < 8; i++)
		{
			DirectX::XMStoreFloat3(&worldVertices[i].position, DirectX::XMVector3Transform(vertices[i], world));
			DirectX::XMStoreFloat4(&worldVertices[i].color, isTrigger ? DirectX::Colors::Yellow : DirectX::Colors::LawnGreen);
		}

		primitiveBatch.DrawIndexed(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, indices, 24, worldVertices, 8);
	}
}

void BoxCollider::SetSize(const DirectX::SimpleMath::Vector3& _size)
{
	size = _size;

	// TODO: Access colliderShape and update size
}
