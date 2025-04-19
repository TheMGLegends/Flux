#include "Visualizer.h"

#include <d3d11.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "Core/Configs/DirectXConfig.h"
#include "Core/Configs/EditorConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/Model.h"

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	using namespace DirectXConfig;

	Visualizer::Visualizer(GameObject* _gameObject) : Component(_gameObject), model(nullptr), textureName("DefaultTexture")
	{
		name = "Visualizer";
		componentType = ComponentType::Visualizer;

		// INFO: Load Default Model & Material 
		SetModel("Cube");
		material = AssetHandler::GetMaterial(ShaderType::Unlit);
	}

	Visualizer::~Visualizer() = default;

	void Visualizer::DrawDetails()
	{
		ImGui::PushID(this);

		// INFO: Active Checkbox
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
		if (ImGui::Checkbox("##ComponentActive", &isActive))
		{
			EditorConfig::SetSceneNeedsSaving(true);
		}

		ImGui::SameLine();
		bool treeOpened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow);

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
				EditorConfig::SetSceneNeedsSaving(true);
			}
		}
		ImGui::PopStyleVar(2);

		// INFO: Model & Texture Selector
		DrawModelAndTextureSelector(treeOpened);
		
		ImGui::PopID();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
	}

	void Visualizer::Serialize(nlohmann::flux_json& json) const
	{
		// INFO: Serialize Parent Class
		Component::Serialize(json);

		auto& jsonBack = json["Components"].back();
		jsonBack["ModelName"] = modelName;
		jsonBack["TextureName"] = textureName;
	}

	void Visualizer::Deserialize(const nlohmann::flux_json& json)
	{
		// INFO: Deserialize Parent Class
		Component::Deserialize(json);

		// INFO: Deserialize Visualizer Data
		SetModel(json["ModelName"].get<std::string>());
		SetMaterialTexture(json["TextureName"].get<std::string>());
	}

	void Visualizer::SetModel(std::string_view _modelName)
	{
		modelName = _modelName;

		model = AssetHandler::GetModel(modelName);

		if (!model)
		{
			Debug::LogError("Visualizer::SetModel() - Failed to load Visualizer Model: " + modelName);
			modelName = "";
		}
	}

	void Visualizer::SetMaterialTexture(std::string_view _textureName)
	{
		textureName = _textureName;

		material.SetTexture(textureName);
	}

	Material& Visualizer::GetMaterial()
	{
		return material;
	}

	void Visualizer::Draw(ID3D11DeviceContext& deviceContext)
	{
		if (!model)
		{
			Debug::LogError("Visualizer::Draw() - Model is nullptr");
			return;
		}

		material.Bind(deviceContext);
		model->Draw(deviceContext);
	}

	void Visualizer::DrawModelAndTextureSelector(bool treeOpened)
	{
		if (!treeOpened) { return; }

		// INFO: Model Selector + Drag & Drop Field
		ImGui::Text("Model");
		ImGui::SameLine();
		ImGui::SetCursorPosX(136.0f);
		ImGui::SetNextItemWidth(150.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		if (ImGui::BeginCombo("##Models", modelName.c_str(), ImGuiComboFlags_HeightLarge))
		{
			for (const auto& [name, modelPtr] : AssetHandler::GetModels())
			{
				if (ImGui::Selectable(name.c_str(), modelName == name))
				{
					SetModel(name);
					EditorConfig::SetSceneNeedsSaving(true);
				}
			}
			ImGui::EndCombo();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Model"))
			{
				std::string payloadModelName = static_cast<const char*>(payload->Data);
				SetModel(payloadModelName);
				EditorConfig::SetSceneNeedsSaving(true);
			}

			ImGui::EndDragDropTarget();
		}

		// INFO: Model Texture Selector + Drag & Drop Field
		ImGui::Text("Texture");
		ImGui::SameLine();
		ImGui::SetCursorPosX(136.0f);
		ImGui::SetNextItemWidth(150.0f);
		if (ImGui::BeginCombo("##ModelTexture", textureName.c_str(), ImGuiComboFlags_HeightLarge))
		{
			for (const auto& [name, texture] : AssetHandler::GetTextures())
			{
				if (ImGui::Selectable(name.c_str(), textureName == name))
				{
					SetMaterialTexture(name);
					EditorConfig::SetSceneNeedsSaving(true);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopStyleVar();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Texture"))
			{
				std::string payloadTextureName = static_cast<const char*>(payload->Data);
				SetMaterialTexture(payloadTextureName);
				EditorConfig::SetSceneNeedsSaving(true);
			}

			ImGui::EndDragDropTarget();
		}

		ImGui::TreePop();
	}
}
