#include "CameraComponent.h"

#include "../GameObject.h"

using namespace DirectX::SimpleMath;

CameraComponent::CameraComponent() : rotation(Quaternion::CreateFromYawPitchRoll(DirectX::XM_PI, 0.0f, 0.0f)), verticalFOV(90.0f), nearClippingPlane(0.1f), farClippingPlane(100.0f), aspectRatio(16.0f / 9.0f), backgroundColour({ 0.0f, 0.0f, 0.0f, 0.0f })
{
	transform = GetGameObject()->GetComponent<TransformComponent>();

	if (transform.expired())
	{
		// TOOD: Logging System Log Error Message
	}

	// TODO: Retrieve aspect ratio of screen
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize CameraComponent
}

void CameraComponent::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize CameraComponent
}

void CameraComponent::DrawWireframe(ID3D11DeviceContext* deviceContext)
{
	// TODO: Debug Draw View Frustum
}

DirectX::XMMATRIX CameraComponent::GetViewMatrix() const
{
	DirectX::XMFLOAT3 position(0.0, 0.0f, 0.0f);

	if (transform.expired())
	{
		// TODO: Logging System Log Error Message
	}
	else
	{
		DirectX::XMStoreFloat3(&position, transform.lock()->GetPosition());
	}

	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR lookTo = DirectX::XMVectorAdd(eye, Forward());
	DirectX::XMVECTOR up = Up();

	return DirectX::XMMatrixLookAtLH(eye, lookTo, up);
}

DirectX::XMMATRIX CameraComponent::GetProjectionMatrix() const
{
	return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(verticalFOV), 
							                 aspectRatio, nearClippingPlane, farClippingPlane);
}

Vector3 CameraComponent::Forward() const
{
	return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 CameraComponent::Right() const
{
	return Vector3::Transform(Vector3::Right, rotation);
}

Vector3 CameraComponent::Up() const
{
	return Vector3::Transform(Vector3::Up, rotation);
}
