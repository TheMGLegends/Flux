#include "Component.h"

#include <imgui_internal.h>
#include <SimpleMath.h>

using namespace Flux;
using namespace DirectX::SimpleMath;

Component::Component(GameObject* _gameObject) : isActive(true), canHaveMultiple(false), isRemoveable(true), gameObject(_gameObject), 
										        name(""), componentType(ComponentType::None)
{
}

Component::~Component()
{
}

void Component::Serialize(nlohmann::flux_json& json) const
{
	// INFO: Serialize Component Data
	json["Components"].push_back({
		{"Name", name},
		{"IsActive", isActive},
		{"ComponentType", componentType}
		});
}

void Component::Deserialize(const nlohmann::flux_json& json)
{
	// INFO: Deserialize Component Data
	name = json["Name"].get<std::string>();
	isActive = json["IsActive"].get<bool>();
}

void Component::SetIsActive(bool _isActive)
{
	isActive = _isActive;
}

bool Component::DisplayVector3Field(const char* label, DirectX::SimpleMath::Vector3& value, float speed, const char* format)
{
	bool changed = false;

	if (ImGui::BeginTable(label, 2))
	{
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f); // INFO: Label Column
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 300.0f); // INFO: Value Column

		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text(label);

		ImGui::TableNextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

		// INFO: X Value
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.75f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::Button("X");
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##X", &value.x, speed, 0.0f, 0.0f, format))
		{
			changed = true;
		}
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// INFO: Y Value
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::Button("Y");
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##Y", &value.y, speed, 0.0f, 0.0f, format))
		{
			changed = true;
		}
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
		ImGui::SameLine();

		// INFO: Z Value
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.75f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.75f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.75f, 1.0f));

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::Button("Z");
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		if (ImGui::DragFloat("##Z", &value.z, speed, 0.0f, 0.0f, format))
		{
			changed = true;
		}
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		ImGui::EndTable();
	}

	return changed;
}
