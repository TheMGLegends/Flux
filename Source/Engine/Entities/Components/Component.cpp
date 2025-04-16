#include "Component.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <SimpleMath.h>

#include "Core/Configs/EngineConfig.h"

#include "Core/Input/Input.h"
#include "Core/Debug/Debug.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	Component::Component(GameObject* _gameObject) : isActive(true), canHaveMultiple(false), isRemoveable(true), gameObject(_gameObject),
													name(""), componentType(ComponentType::None), draggingBox(false)
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
		if (isActive != _isActive) { isActive = _isActive; }
	}

	bool Component::DisplayVector3Field(const char* label, DirectX::SimpleMath::Vector3& value, float speed, const char* format, float min, float max)
	{
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
			if (ImGui::DragFloat("##X", &value.x, speed, min, max, format, ImGuiSliderFlags_ClampOnInput))
			{
				draggingBox = true;
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
				draggingBox = true;
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
				draggingBox = true;
			}
			ImGui::PopStyleVar();
			ImGui::PopItemWidth();

			ImGui::EndTable();
		}

		// INFO: Prepare mouse warping if the field was changed
		if (draggingBox)
		{
			Vector2 mousePosition = Input::GetMousePosition();
			ImGuiIO& io = ImGui::GetIO();

			if (mousePosition.x <= 1.0f)
			{
				mousePosition.x = EngineConfig::GetWindowWidth() - 2.0f;
				Input::SetMousePosition(mousePosition);
				io.MousePos = ImVec2(mousePosition.x, mousePosition.y);
				io.MouseDelta = ImVec2(0.0f, 0.0f);
			}
			else if (mousePosition.x >= EngineConfig::GetWindowWidth() - 1.0f)
			{
				mousePosition.x = 2.0f;
				Input::SetMousePosition(mousePosition);
				io.MousePos = ImVec2(mousePosition.x, mousePosition.y);
				io.MouseDelta = ImVec2(0.0f, 0.0f);
			}

			if (Input::GetMouseButtonUp(SDL_BUTTON_LEFT))
			{
				draggingBox = false;
			}
		}

		return draggingBox;
	}
}
