#include "SphereCollider.h"

#include <DirectXColors.h>

#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"
#include "Engine/Entities/Components/Transform.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

SphereCollider::SphereCollider(GameObject* _gameObject) : Collider(_gameObject), radius(1.0f)
{
	name = "SphereCollider";
	componentType = ComponentType::SphereCollider;

	auto& physics = Physics::GetPhysics();

	// INFO: Create Sphere Collider Shape
	colliderShape = physics.createShape(physx::PxSphereGeometry(radius), Physics::GetDefaultPhysicsMaterial());

	// INFO: Search for existing physics body component
	GameObject* owningGameObject = GetGameObject();

	if (owningGameObject)
	{
		// INFO: Setup as rigid static actor
		if (!owningGameObject->HasComponent<PhysicsBody>())
		{
			const Vector3& position = owningGameObject->transform.lock()->GetPosition();
			rigidStatic = physics.createRigidStatic(physx::PxTransform(position.x, position.y, position.z, physx::PxIdentity));
			rigidStatic->attachShape(*colliderShape);
			SceneContext::GetScene().GetPhysicsScene().addActor(*rigidStatic);
		}
	}
}

SphereCollider::~SphereCollider()
{
	if (rigidStatic)
	{
		rigidStatic->release();
		rigidStatic = nullptr;
	}
}

void SphereCollider::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Collider::Serialize(json);

	json["Components"].back()["Radius"] = radius;
}

void SphereCollider::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Collider::Deserialize(json);

	// INFO: Deserialize SphereCollider Data
	radius = json["Radius"].get<float>();
}

void SphereCollider::DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
{
	DirectX::XMVECTOR centre = DirectX::XMLoadFloat3(&GetGameObject()->transform.lock()->GetPosition());

	DirectX::XMVECTOR xRange = DirectX::XMVectorSet(radius, 0, 0, 0);
	DirectX::XMVECTOR yRange = DirectX::XMVectorSet(0, radius, 0, 0);
	DirectX::XMVECTOR zRange = DirectX::XMVectorSet(0, 0, radius, 0);

	bool isTrigger = IsTrigger();

	DrawRing(deviceContext, primitiveBatch, centre, xRange, yRange, isTrigger);
	DrawRing(deviceContext, primitiveBatch, centre, xRange, zRange, isTrigger);
	DrawRing(deviceContext, primitiveBatch, centre, yRange, zRange, isTrigger);
}

void SphereCollider::SetRadius(float _radius)
{
	radius = _radius;

	// TODO: Access colliderShape and update radius
}

void Flux::SphereCollider::DrawRing(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch, 
									const DirectX::XMVECTOR& centre, const DirectX::XMVECTOR& majorAxis, const DirectX::XMVECTOR& minorAxis, bool isTrigger)
{
	static const int ringSegments = 32;

	DirectX::VertexPositionColor vertices[ringSegments + 1]{};

	float segmentAngle = DirectX::XM_2PI / static_cast<float>(ringSegments);
	DirectX::XMVECTOR cosSegment = DirectX::XMVectorReplicate(cosf(segmentAngle));
	DirectX::XMVECTOR sinSegment = DirectX::XMVectorReplicate(sinf(segmentAngle));
	DirectX::XMVECTOR incrementalSin = DirectX::XMVectorZero();

	static const DirectX::XMVECTOR initialCos = { 1.0f, 1.0f, 1.0f, 1.0f };

	DirectX::XMVECTOR incrementalCos = initialCos;

	for (size_t i = 0; i < ringSegments; i++)
	{
		DirectX::XMVECTOR position{};

		position = DirectX::XMVectorMultiplyAdd(majorAxis, incrementalCos, centre);
		position = DirectX::XMVectorMultiplyAdd(minorAxis, incrementalSin, position);

		XMStoreFloat3(&vertices[i].position, position);
		XMStoreFloat4(&vertices[i].color, isTrigger ? DirectX::Colors::Yellow : DirectX::Colors::LawnGreen);

		DirectX::XMVECTOR newCos = DirectX::XMVectorSubtract(DirectX::XMVectorMultiply(incrementalCos, cosSegment), DirectX::XMVectorMultiply(incrementalSin, sinSegment));
		DirectX::XMVECTOR newSin = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(incrementalCos, sinSegment), DirectX::XMVectorMultiply(incrementalSin, cosSegment));

		incrementalCos = newCos;
		incrementalSin = newSin;
	}

	vertices[ringSegments] = vertices[0];

	primitiveBatch.Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP, vertices, static_cast<size_t>(ringSegments) + 1);
}
