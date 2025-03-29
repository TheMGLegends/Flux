#include "BoxCollider.h"

#include <DirectXColors.h>

#include "Engine/Physics/Physics.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/GameObjects/GameObject.h"
#include "Engine/Entities/Components/PhysicsBody.h"
#include "Engine/Entities/Components/Transform.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

BoxCollider::BoxCollider(GameObject* _gameObject) : Collider(_gameObject), size(Vector3::One)
{
	name = "BoxCollider";
	componentType = ComponentType::BoxCollider;

	// INFO: Create Box Collider Shape
	SetColliderShape();
}

BoxCollider::~BoxCollider()
{
	if (rigidActor)
	{
		rigidActor->release();
		rigidActor = nullptr;
	}
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

	// INFO: Deserialize BoxCollider Data
	SetSize(Vector3(json["Size"][0].get<float>(), json["Size"][1].get<float>(), json["Size"][2].get<float>()));
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
		// INFO: Add a small offset to prevent Z-Fighting
		Vector3 offsetSize = size;
		offsetSize.x += 0.01f;
		offsetSize.y += 0.01f;
		offsetSize.z += 0.01f;

		DirectX::XMMATRIX world = owningTransform->GetWorldMatrix(offsetSize);

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

void BoxCollider::SetColliderShape()
{
	// INFO: Clear Collider Shape if it already exists
	if (colliderShape)
	{
		colliderShape->release();
		colliderShape = nullptr;
	}

	// INFO: Create Box Collider Shape
	colliderShape = Physics::GetPhysics().createShape(physx::PxBoxGeometry(size.x, size.y, size.z), Physics::GetDefaultPhysicsMaterial(), true);

	// INFO: Default Collisions with everything
	physx::PxFilterData filterData{};
	filterData.word0 = 1;
	filterData.word1 = 1;
	colliderShape->setSimulationFilterData(filterData);

	// INFO: Reset trigger state
	SetIsTrigger(IsTrigger());

	// INFO: Ensure Rigid Actor is valid
	if (rigidActor)
	{
		if (!rigidActor->attachShape(*colliderShape))
		{
			Debug::LogError("BoxCollider::BoxCollider() - Failed to attach shape to Rigid Actor");
		}

		SceneContext::GetScene().GetPhysicsScene().addActor(*rigidActor);
	}
}

void BoxCollider::SetSize(const DirectX::SimpleMath::Vector3& _size)
{
	size = _size;

	if (colliderShape)
	{
		// INFO: Get reference to actor that the shape is attached to
		physx::PxRigidActor* rigidActor = colliderShape->getActor();

		// INFO: Remove the shape from the actor to allow for geometry changes
		rigidActor->detachShape(*colliderShape);

		physx::PxBoxGeometry boxGeometry{};
		if (colliderShape->getBoxGeometry(boxGeometry))
		{
			boxGeometry.halfExtents = { size.x, size.y, size.z };
			colliderShape->setGeometry(boxGeometry);
		}

		// INFO: Re-attach the shape to the actor
		rigidActor->attachShape(*colliderShape);
	}
}
