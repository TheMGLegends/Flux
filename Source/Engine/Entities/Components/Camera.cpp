#include "Camera.h"

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/ConstantBuffers.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Model.h"
#include "Engine/Entities/GameObject.h"

using namespace Flux;
using namespace Flux::ConstantBuffers;
using namespace Flux::DirectXConfig;
using namespace DirectX::SimpleMath;

Camera::Camera(GameObject* _gameObject) : Component(_gameObject), rotation(Quaternion::CreateFromYawPitchRoll(DirectX::XM_PI, 0.0f, 0.0f)), verticalFOV(90.0f), 
									      nearClippingPlane(0.1f), farClippingPlane(100.0f), aspectRatio(16.0f / 9.0f), backgroundColour({ 1.0f, 1.0f, 1.0f, 0.0f }), 
										  useSkybox(true)
{
	componentType = ComponentType::Camera;

	transform = GetGameObject()->GetComponent<Transform>();

	if (transform.expired())
	{
		Debug::LogError("Camera::Camera() - Camera Component must be attached to a GameObject with a Transform Component");
	}

	// INFO: Load Default Skybox Model & Material
	SetSkyboxModel("Cube");
	skyboxMaterial = AssetHandler::GetMaterial(ShaderType::Skybox);

	// TODO: TESTING
	skyboxMaterial->SetTexture("DebugSkybox");

	// TODO: Retrieve aspect ratio of screen
}

Camera::~Camera()
{
}

void Camera::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	// TODO: Serialize CameraComponent
}

void Camera::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// TODO: Deserialize CameraComponent
}

void Camera::DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
{
	// TODO: Debug Draw View Frustum
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	DirectX::XMFLOAT3 position(0.0, 0.0f, 0.0f);

	if (transform.expired())
	{
		Debug::LogError("Camera::GetViewMatrix() - Transform Component has expired");
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

DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(verticalFOV), 
							                 aspectRatio, nearClippingPlane, farClippingPlane);
}

Vector3 Camera::Forward() const
{
	return Vector3::Transform(Vector3::Forward, rotation);
}

Vector3 Camera::Right() const
{
	return Vector3::Transform(Vector3::Right, rotation);
}

Vector3 Camera::Up() const
{
	return Vector3::Transform(Vector3::Up, rotation);
}

void Flux::Camera::SetSkyboxModel(const std::string& modelName)
{
	skyboxModel = AssetHandler::GetModel(modelName);

	if (!skyboxModel)
		Debug::LogError("Camera::SetSkyboxModel() - Failed to load Skybox Model: " + modelName);
}

void Camera::DrawSkybox(ID3D11DeviceContext& deviceContext, const DirectX::XMMATRIX& translation, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	if (!useSkybox)
		return;

	if (!skyboxModel || !skyboxMaterial)
	{
		Debug::LogError("Camera::DrawSkybox() - Skybox Model or Material is nullptr");
		return;
	}

	UnlitVS skyboxVS{};
	skyboxVS.wvp = translation * view * projection;

	if (skyboxMaterial->GetConstantBufferType() == ConstantBufferType::Unlit)
		deviceContext.UpdateSubresource(skyboxMaterial->GetConstantBuffer(), 0, nullptr, &skyboxVS, 0, 0);

	skyboxMaterial->Bind(deviceContext);
	skyboxModel->Draw(deviceContext);
}
