#include "TransformComponent.h"

using namespace DirectX::SimpleMath;

TransformComponent::TransformComponent() : position(Vector3::Zero), rotation(Quaternion::Identity), scale(Vector3::One)
{
	isRemoveable = false;
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize TransformComponent
}

void TransformComponent::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize TransformComponent
}

DirectX::XMMATRIX TransformComponent::GetWorldMatrix() const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

	return scaleMatrix * rotationMatrix * translationMatrix;
}

DirectX::XMMATRIX TransformComponent::GetWorldMatrix(const Vector3& additionalScale) const
{
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(position);

	Vector3 newScale = scale + additionalScale;
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(newScale);

	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation);

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
