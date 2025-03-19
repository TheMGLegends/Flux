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
	
	auto& physics = Physics::GetPhysics();

	// INFO: Create Box Collider Shape
	colliderShape = physics.createShape(physx::PxBoxGeometry(size.x, size.y, size.z), Physics::GetDefaultPhysicsMaterial(), true);

	// INFO: Search for existing physics body component
	GameObject* owningGameObject = GetGameObject();

	if (owningGameObject)
	{
		// INFO: Setup as rigid static actor
		if (!owningGameObject->HasComponent<PhysicsBody>())
		{
			const Vector3& position = owningGameObject->transform.lock()->GetPosition();
			const Quaternion& rotation = owningGameObject->transform.lock()->GetRotation();
			rigidStatic = physics.createRigidStatic(physx::PxTransform(position.x, position.y, position.z, 
													physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));

			if (!rigidStatic->attachShape(*colliderShape))
				Debug::LogError("BoxCollider::BoxCollider() - Failed to attach shape to Rigid Static Actor");

			SceneContext::GetScene().GetPhysicsScene().addActor(*rigidStatic);
		}
	}
}

BoxCollider::~BoxCollider()
{
	if (rigidStatic)
	{
		rigidStatic->release();
		rigidStatic = nullptr;
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
