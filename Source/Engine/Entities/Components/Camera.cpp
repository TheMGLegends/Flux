#include "Camera.h"

#include <DirectXColors.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/EngineConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/ConstantBuffers.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Model.h"

#include "Engine/Entities/GameObjects/GameObject.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	using namespace ConstantBuffers;
	using namespace DirectXConfig;

	Camera::Camera(GameObject* _gameObject) : Component(_gameObject), verticalFOV(90.0f), nearClippingPlane(0.1f), farClippingPlane(100.0f),
		aspectRatio(EngineConfig::ASPECT_RATIO), backgroundColour(0.5f, 0.5f, 0.5f, 1.0f),
		shouldDrawFrustum(true), skyboxTextureName("DefaultSkybox"), useSkybox(true)
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

		// INFO: Initialise Bounding Frustum
		SetFrustum();
	}

	Camera::~Camera()
	{
	}

	void Camera::DrawDetails()
	{
		ImGui::PushID(this);

		bool treeOpened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		// INFO: Active Checkbox
		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Checkbox("##ComponentActive", &isActive))
		{
			EditorConfig::sceneNeedsSaving = true;
		}
		ImGui::PopStyleVar();

		// INFO: Remove Component Button
		ImVec2 buttonSize = ImVec2(65.0f, 0.0f);
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonSize.x + 10.0f));
		if (ImGui::Button("Remove", buttonSize))
		{
			GameObject* gameObject = GetGameObject();
			if (gameObject) 
			{ 
				gameObject->RemoveComponent(weak_from_this()); 
				EditorConfig::sceneNeedsSaving = true;
			}
		}
		ImGui::PopStyleVar();

		if (treeOpened)
		{
			// INFO: Use Skybox Checkbox
			ImGui::Text("Use Skybox");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			if (ImGui::Checkbox("##UseSkybox", &useSkybox))
			{
				EditorConfig::sceneNeedsSaving = true;
			}

			// INFO: Skybox Texture Selector + Drag & Drop Field
			ImGui::Text("Texture");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(150.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
			if (ImGui::BeginCombo("##SkyboxTexture", skyboxTextureName.c_str(), ImGuiComboFlags_HeightLarge))
			{
				for (const auto& skyboxTexture : AssetHandler::GetSkyboxTextures())
				{
					if (ImGui::Selectable(skyboxTexture.first.c_str(), skyboxTextureName == skyboxTexture.first))
					{
						SetMaterialTexture(skyboxTexture.first);
						EditorConfig::sceneNeedsSaving = true;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopStyleVar();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SkyboxTexture"))
				{
					std::string textureName = static_cast<const char*>(payload->Data);
					SetMaterialTexture(textureName);
					EditorConfig::sceneNeedsSaving = true;
				}

				ImGui::EndDragDropTarget();
			}

			// INFO: Background Colour Picker
			ImGuiColorEditFlags colourEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;
			ImGui::Text("Background");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			if (ImGui::ColorEdit4("##BackgroundColour", &backgroundColour[0], colourEditFlags))
			{
				EditorConfig::sceneNeedsSaving = true;
			}

			// INFO: FOV Slider
			ImGui::Text("Vertical FOV");
			ImGui::SameLine();
			ImGui::SetCursorPosX(136.0f);
			ImGui::SetNextItemWidth(200.0f);
			if (ImGui::SliderFloat("##VerticalFOV", &verticalFOV, 1.0f, 150.0f, "%.1f", ImGuiSliderFlags_ClampOnInput))
			{
				EditorConfig::sceneNeedsSaving = true;
			}

			// INFO: Clipping Planes
			ImGui::Text("Clipping Planes");
			if (ImGui::BeginTable("##ClippingPlanes", 2))
			{
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 81.0f); // INFO: Label Column
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 75.0f); // INFO: Value Column

				ImGui::AlignTextToFramePadding();

				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(50.0f);
				ImGui::Text("Near");
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(136.0f);
				if (ImGui::InputFloat("##NearClippingPlane", &nearClippingPlane, 0.0f, 0.0f, "%.1f"))
				{
					EditorConfig::sceneNeedsSaving = true;
				}

				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(50.0f);
				ImGui::Text("Far");
				ImGui::TableNextColumn();
				ImGui::SetCursorPosX(136.0f);
				if (ImGui::InputFloat("##FarClippingPlane", &farClippingPlane, 0.0f, 0.0f, "%.1f"))
				{
					EditorConfig::sceneNeedsSaving = true;
				}

				ImGui::EndTable();
			}

			ImGui::TreePop();
		}
		ImGui::PopID();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
	}

	void Camera::Serialize(nlohmann::flux_json& json) const
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

	void Camera::Deserialize(const nlohmann::flux_json& json)
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
		useSkybox = json["UseSkybox"].get<bool>();

		// INFO: Initialise Bounding Frustum
		SetFrustum();
	}

	void Camera::DrawWireframe(ID3D11DeviceContext& deviceContext, DirectX::PrimitiveBatch<DirectX::VertexPositionColor>& primitiveBatch)
	{
		if (!shouldDrawFrustum) { return; }

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
			return DirectX::XMMatrixIdentity();
		}

		std::shared_ptr<Transform> sharedTransform = transform.lock();
		DirectX::XMStoreFloat3(&position, sharedTransform->GetPosition());

		DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&position);
		DirectX::XMVECTOR lookTo = DirectX::XMVectorAdd(eye, sharedTransform->Forward());
		DirectX::XMVECTOR up = sharedTransform->Up();

		return DirectX::XMMatrixLookAtLH(eye, lookTo, up);
	}

	DirectX::XMMATRIX Camera::GetProjectionMatrix() const
	{
		return DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(verticalFOV),
			aspectRatio, nearClippingPlane, farClippingPlane);
	}

	void Camera::SetSkyboxModel(const std::string& modelName)
	{
		skyboxModel = AssetHandler::GetModel(modelName);

		if (!skyboxModel)
		{
			Debug::LogError("Camera::SetSkyboxModel() - Failed to load Skybox Model: " + modelName);
		}
	}

	void Camera::SetMaterialTexture(const std::string& _textureName)
	{
		skyboxTextureName = _textureName;

		skyboxMaterial.SetTexture(skyboxTextureName, true);
	}

	void Camera::DrawSkybox(ID3D11DeviceContext& deviceContext, const DirectX::XMMATRIX& translation, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
	{
		if (!useSkybox) { return; }

		if (!skyboxModel)
		{
			Debug::LogError("Camera::DrawSkybox() - Skybox Model is nullptr");
			return;
		}

		UnlitVS skyboxVS{};
		skyboxVS.wvp = translation * view * projection;

		if (skyboxMaterial.GetConstantBufferType() == ConstantBufferType::Unlit)
		{
			deviceContext.UpdateSubresource(skyboxMaterial.GetConstantBuffer(), 0, nullptr, &skyboxVS, 0, 0);
		}

		skyboxMaterial.Bind(deviceContext);
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
		return DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(verticalFOV),
			aspectRatio, _nearClippingPlane, _farClippingPlane);
	}
}
