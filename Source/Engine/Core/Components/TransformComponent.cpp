#include "TransformComponent.h"

using namespace DirectX::SimpleMath;

TransformComponent::TransformComponent() : position(Vector3::Zero), rotation(Quaternion::Identity), scale(Vector3::One)
{
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize Transform
}

void TransformComponent::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize Transform
}

DirectX::XMMATRIX TransformComponent::GetWorldMatrix() const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

	// INFO: Returns the World Matrix (World = Scale * Rotation * Translation)
	return scaleMatrix * rotationMatrix * translationMatrix;
}

DirectX::XMMATRIX TransformComponent::GetWorldMatrix(const Vector3& additionalScale) const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);

	Vector3 newScale = scale + additionalScale;
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(newScale);

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

	// INFO: Returns the World Matrix (World = Scale * Rotation * Translation)
	return scaleMatrix * rotationMatrix * translationMatrix;
}

Vector3 TransformComponent::Forward() const
{
	return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 TransformComponent::Right() const
{
	return Vector3::Transform(Vector3::Right, rotation);
}

Vector3 TransformComponent::Up() const
{
	return Vector3::Transform(Vector3::Up, rotation);
}

void TransformComponent::Rotate(const Vector3& eulerRotation)
{
	Quaternion rotationQuaternion = Quaternion::CreateFromYawPitchRoll(eulerRotation.y, eulerRotation.x, eulerRotation.z);

	// INFO: Combine the current rotation with the new rotation
	rotation *= rotationQuaternion;
}
