#include "Transform.h"

#pragma warning (push, 0)
#include <PxRigidDynamic.h>
#pragma warning (pop)

#include "PhysicsBody.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

Transform::Transform(GameObject* _gameObject) : Component(_gameObject), position(Vector3::Zero), rotation(Quaternion::Identity), scale(Vector3::One)
{
	name = "Transform";
	componentType = ComponentType::Transform;
	isRemoveable = false;
}

Transform::~Transform()
{
}

void Transform::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	auto& jsonBack = json["Components"].back();
	jsonBack["Position"] = { position.x, position.y, position.z };

	Vector3 eulerRotation = rotation.ToEuler();
	jsonBack["Rotation"] = { DirectX::XMConvertToDegrees(eulerRotation.x),
							 DirectX::XMConvertToDegrees(eulerRotation.y),
							 DirectX::XMConvertToDegrees(eulerRotation.z) }; // INFO: Rotation Values saved in degrees for readability
	jsonBack["Scale"] = { scale.x, scale.y, scale.z };
}

void Transform::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// INFO: Deserialize Transform Data
	position = Vector3(json["Position"][0].get<float>(), json["Position"][1].get<float>(), json["Position"][2].get<float>());
	rotation = Quaternion::CreateFromYawPitchRoll(DirectX::XMConvertToRadians(json["Rotation"][1].get<float>()),
												  DirectX::XMConvertToRadians(json["Rotation"][0].get<float>()),
												  DirectX::XMConvertToRadians(json["Rotation"][2].get<float>())); // INFO: In degrees so need to convert to radians
	scale = Vector3(json["Scale"][0].get<float>(), json["Scale"][1].get<float>(), json["Scale"][2].get<float>());
}

DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

	return scaleMatrix * rotationMatrix * translationMatrix;
}

DirectX::XMMATRIX Transform::GetWorldMatrix(const Vector3& otherScale) const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(otherScale);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

	return scaleMatrix * rotationMatrix * translationMatrix;
}

Vector3 Transform::Forward() const
{
	return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 Transform::Right() const
{
	return Vector3::Transform(Vector3::Right, rotation);
}

Vector3 Transform::Up() const
{
	return Vector3::Transform(Vector3::Up, rotation);
}

void Transform::Rotate(const Vector3& eulerRotation)
{
	Quaternion rotationQuaternion = Quaternion::CreateFromYawPitchRoll(eulerRotation.y, eulerRotation.x, eulerRotation.z);

	// INFO: Combine the current rotation with the new rotation
	rotation *= rotationQuaternion;
}

void Transform::SetPositionEditor(const DirectX::SimpleMath::Vector3& _position)
{
	position = _position;

	// INFO: If game object has a physics body we need to setGlobalPose
	if (GetGameObject()->HasComponent<PhysicsBody>())
	{
		auto rigidDynamic = GetGameObject()->GetComponent<PhysicsBody>().lock()->VerifyRigidActor();

		if (rigidDynamic)
		{
			rigidDynamic->setGlobalPose(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z), 
										physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
		}
	}
}

void Transform::SetRotationEditor(const DirectX::SimpleMath::Quaternion& _rotation)
{
	rotation = _rotation;

	// INFO: If game object has a physics body we need to setGlobalPose
	if (GetGameObject()->HasComponent<PhysicsBody>())
	{
		auto rigidDynamic = GetGameObject()->GetComponent<PhysicsBody>().lock()->VerifyRigidActor();
		if (rigidDynamic)
		{
			rigidDynamic->setGlobalPose(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z),
										physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
		}
	}
}
