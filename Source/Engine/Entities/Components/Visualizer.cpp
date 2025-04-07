#include "Visualizer.h"

#include <imgui_internal.h>

#include "Core/Configs/DirectXConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/Material.h"
#include "Core/Renderer/Model.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace Flux::DirectXConfig;

Visualizer::Visualizer(GameObject* _gameObject) : Component(_gameObject), model(nullptr), textureName("DefaultTexture")
{
	name = "Visualizer";
	componentType = ComponentType::Visualizer;

	// INFO: Load Default Model & Material 
	SetModel("Cube");
	material = AssetHandler::GetMaterial(ShaderType::Unlit);
}

Visualizer::~Visualizer()
{
}

void Visualizer::DrawDetails()
{
	ImGui::PushID(this);

	bool treeOpened = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	// INFO: Active Checkbox
	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::Checkbox("##ComponentActive", &isActive);
	ImGui::PopStyleVar();

	// INFO: Remove Component Button
	ImVec2 buttonSize = ImVec2(65.0f, 0.0f);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - (buttonSize.x + 10.0f));
	if (ImGui::Button("Remove", buttonSize))
	{
		GameObject* gameObject = GetGameObject();
		if (gameObject) { gameObject->RemoveComponent(weak_from_this()); }
	}

	if (treeOpened)
	{
		// INFO: Model Selector // TODO: NOT FINAL, NEED DRAG & DROP
		ImGui::Text("Model");
		ImGui::SameLine();
		ImGui::SetCursorPosX(136.0f);
		ImGui::SetNextItemWidth(200.0f);
		if (ImGui::BeginCombo("##Models", modelName.c_str(), ImGuiComboFlags_HeightLarge))
		{
			for (const auto& model : AssetHandler::GetModels())
			{
				if (ImGui::Selectable(model.first.c_str(), modelName == model.first))
				{
					SetModel(model.first);
				}
			}
			ImGui::EndCombo();
		}

		// INFO: Model Texture Selector // TODO: NOT FINAL, NEED DRAG & DROG
		ImGui::Text("Texture");
		ImGui::SameLine();
		ImGui::SetCursorPosX(136.0f);
		ImGui::SetNextItemWidth(200.0f);
		if (ImGui::BeginCombo("##ModelTexture", textureName.c_str(), ImGuiComboFlags_HeightLarge))
		{
			for (const auto& texture : AssetHandler::GetTextures())
			{
				if (ImGui::Selectable(texture.first.c_str(), textureName == texture.first))
				{
					SetMaterialTexture(texture.first);
				}
			}
			ImGui::EndCombo();
		}

		ImGui::TreePop();
	}
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

void Visualizer::SetModel(const std::string& _modelName)
{
	modelName = _modelName;

	model = AssetHandler::GetModel(modelName);

	if (!model)
	{
		Debug::LogError("Visualizer::SetModel() - Failed to load Visualizer Model: " + modelName);
		modelName = "";
	}
}

void Visualizer::SetMaterialTexture(const std::string& _textureName)
{
	textureName = _textureName;

	material.SetTexture(textureName);
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
