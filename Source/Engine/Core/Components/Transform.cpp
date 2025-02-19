#include "Transform.h"

using namespace DirectX::SimpleMath;

Transform::Transform() : position(Vector3::Zero), rotation(Quaternion::Identity), scale(Vector3::One)
{
}

Transform::~Transform()
{
}

void Transform::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize Transform
}

void Transform::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize Transform
}

DirectX::XMMATRIX Transform::GetWorldMatrix() const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

	// INFO: Returns the World Matrix (World = Scale * Rotation * Translation)
	return scaleMatrix * rotationMatrix * translationMatrix;
}

DirectX::XMMATRIX Transform::GetWorldMatrix(const Vector3& additionalScale) const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);

	Vector3 newScale = scale + additionalScale;
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(newScale);

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

	// INFO: Returns the World Matrix (World = Scale * Rotation * Translation)
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
