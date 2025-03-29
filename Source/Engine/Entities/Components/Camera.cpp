#include "Camera.h"

#include "Core/Configs/EngineConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/ConstantBuffers.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Model.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace Flux::ConstantBuffers;
using namespace Flux::DirectXConfig;
using namespace DirectX::SimpleMath;

Camera::Camera(GameObject* _gameObject) : Component(_gameObject), rotation(Quaternion::CreateFromYawPitchRoll(DirectX::XM_PI, 0.0f, 0.0f)), verticalFOV(90.0f), 
									      nearClippingPlane(0.1f), farClippingPlane(100.0f), aspectRatio(EngineConfig::ASPECT_RATIO), backgroundColour({ 0.5f, 0.5f, 0.5f, 1.0f }),
										  shouldDrawFrustum(true), useSkybox(true)
{
	name = "Camera";
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
	SetMaterialTexture("DebugSkybox");

	// INFO: Initialise Bounding Frustum
	SetFrustum();
}

Camera::~Camera()
{
}

void Camera::Serialize(nlohmann::ordered_json& json) const
{
	// INFO: Serialize Parent Class
	Component::Serialize(json);

	auto& jsonBack = json["Components"].back();
	jsonBack["FOV"] = verticalFOV;
	jsonBack["NearClippingPlane"] = nearClippingPlane;
	jsonBack["FarClippingPlane"] = farClippingPlane;
	jsonBack["BackgroundColour"] = { backgroundColour[0], backgroundColour[1], backgroundColour[2], backgroundColour[3] };
	jsonBack["SkyboxTexture"] = skyboxTextureName;
	jsonBack["UseSkybox"] = useSkybox;
}

void Camera::Deserialize(const nlohmann::ordered_json& json)
{
	// INFO: Deserialize Parent Class
	Component::Deserialize(json);

	// INFO: Deserialize Camera Data
	verticalFOV = json["FOV"].get<float>();
	nearClippingPlane = json["NearClippingPlane"].get<float>();
	farClippingPlane = json["FarClippingPlane"].get<float>();

	auto& backgroundColourJson = json["BackgroundColour"];
	for (size_t i = 0; i < backgroundColourJson.size(); i++)
	{
		backgroundColour[i] = backgroundColourJson[i].get<float>();
	}

	SetMaterialTexture(json["SkyboxTexture"].get<std::string>());
	SetUseSkybox(json["UseSkybox"].get<bool>());

	// INFO: Initialise Bounding Frustum
	SetFrustum();
}

void Camera::DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
{
	if (!shouldDrawFrustum)
		return;

	std::shared_ptr<Transform> owningTransform = GetGameObject()->transform.lock();

	if (owningTransform)
	{
		DirectX::XMMATRIX world = owningTransform->GetWorldMatrix();

		DirectX::VertexPositionColor worldFrustumVertices[24]{};

		for (size_t i = 0; i < 24; i++)
		{
			DirectX::XMStoreFloat3(&worldFrustumVertices[i].position, DirectX::XMVector3Transform(frustumVertices[i], world));
			DirectX::XMStoreFloat4(&worldFrustumVertices[i].color, DirectX::Colors::White);
		}

		primitiveBatch.Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, worldFrustumVertices, 24);
	}
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

void Camera::SetSkyboxModel(const std::string& modelName)
{
	skyboxModel = AssetHandler::GetModel(modelName);

	if (!skyboxModel)
		Debug::LogError("Camera::SetSkyboxModel() - Failed to load Skybox Model: " + modelName);
}

void Camera::SetMaterialTexture(const std::string& _textureName)
{
	skyboxTextureName = _textureName;

	skyboxMaterial->SetTexture(skyboxTextureName);
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

void Camera::SetFrustum()
{
	DirectX::BoundingFrustum::CreateFromMatrix(frustum, GetAdjustedProjectionMatrix(nearClippingPlane, farClippingPlane * 0.1f));

	DirectX::XMFLOAT3 corners[DirectX::BoundingFrustum::CORNER_COUNT]{};
	frustum.GetCorners(corners);

	frustumVertices[0] = DirectX::XMLoadFloat3(&corners[0]);
	frustumVertices[1] = DirectX::XMLoadFloat3(&corners[1]);
	frustumVertices[2] = DirectX::XMLoadFloat3(&corners[1]);
	frustumVertices[3] = DirectX::XMLoadFloat3(&corners[2]);
	frustumVertices[4] = DirectX::XMLoadFloat3(&corners[2]);
	frustumVertices[5] = DirectX::XMLoadFloat3(&corners[3]);
	frustumVertices[6] = DirectX::XMLoadFloat3(&corners[3]);
	frustumVertices[7] = DirectX::XMLoadFloat3(&corners[0]);

	frustumVertices[8] = DirectX::XMLoadFloat3(&corners[0]);
	frustumVertices[9] = DirectX::XMLoadFloat3(&corners[4]);
	frustumVertices[10] = DirectX::XMLoadFloat3(&corners[1]);
	frustumVertices[11] = DirectX::XMLoadFloat3(&corners[5]);
	frustumVertices[12] = DirectX::XMLoadFloat3(&corners[2]);
	frustumVertices[13] = DirectX::XMLoadFloat3(&corners[6]);
	frustumVertices[14] = DirectX::XMLoadFloat3(&corners[3]);
	frustumVertices[15] = DirectX::XMLoadFloat3(&corners[7]);

	frustumVertices[16] = DirectX::XMLoadFloat3(&corners[4]);
	frustumVertices[17] = DirectX::XMLoadFloat3(&corners[5]);
	frustumVertices[18] = DirectX::XMLoadFloat3(&corners[5]);
	frustumVertices[19] = DirectX::XMLoadFloat3(&corners[6]);
	frustumVertices[20] = DirectX::XMLoadFloat3(&corners[6]);
	frustumVertices[21] = DirectX::XMLoadFloat3(&corners[7]);
	frustumVertices[22] = DirectX::XMLoadFloat3(&corners[7]);
	frustumVertices[23] = DirectX::XMLoadFloat3(&corners[4]);
}

DirectX::XMMATRIX Camera::GetAdjustedProjectionMatrix(float _nearClippingPlane, float _farClippingPlane) const
{
	return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(verticalFOV),
											 aspectRatio, _nearClippingPlane, _farClippingPlane);
}
