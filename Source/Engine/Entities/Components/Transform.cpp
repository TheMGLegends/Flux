#include "Transform.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

Transform::Transform(GameObject* _gameObject) : Component(_gameObject), position(Vector3::Zero), rotation(Quaternion::Identity), scale(Vector3::One)
{
	isRemoveable = false;
}

Transform::~Transform()
{
}

void Transform::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize TransformComponent
}

void Transform::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize TransformComponent
}

DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

	return scaleMatrix * rotationMatrix * translationMatrix;
}

DirectX::XMMATRIX Transform::GetWorldMatrix(const Vector3& additionalScale) const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);

	Vector3 newScale = scale + additionalScale;
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(newScale);

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
