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
	Component::Component(GameObject* _gameObject) : isActive(true), canHaveMultiple(false), isRemoveable(true),
													name(""), componentType(ComponentType::None), gameObject(_gameObject), draggingBox(false)
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

	GameObject* Component::GetGameObject() const
	{
		return gameObject;
	}

	void Component::SetIsActive(bool _isActive)
	{
		if (isActive != _isActive) { isActive = _isActive; }
	}

	bool Component::IsActive() const
	{
		return isActive;
	}

	bool Component::CanHaveMultiple() const
	{
		return canHaveMultiple;
	}

	bool Component::IsRemoveable() const
	{
		return isRemoveable;
	}

	ComponentType Component::GetComponentType() const
	{
		return componentType;
	}

	bool Component::DisplayVector3Field(const char* label, DirectX::SimpleMath::Vector3& value, float speed, const char* format, float min, float max, bool restrictAll)
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

			float actualMin = restrictAll ? min : 0.0f;
			float actualMax = restrictAll ? max : 0.0f;

			// INFO: Y Value
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.75f, 0.0f, 1.0f));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::Button("Y");
			ImGui::PopStyleColor(3);
			ImGui::SameLine();

			if (ImGui::DragFloat("##Y", &value.y, speed, actualMin, actualMax, format, ImGuiSliderFlags_ClampOnInput))
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
			if (ImGui::DragFloat("##Z", &value.z, speed, actualMin, actualMax, format, ImGuiSliderFlags_ClampOnInput))
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
				mousePosition.x = static_cast<float>(EngineConfig::GetWindowWidth()) - 2.0f;
				Input::SetMousePosition(mousePosition);
				io.MousePos = ImVec2(mousePosition.x, mousePosition.y);
				io.MouseDelta = ImVec2(0.0f, 0.0f);
			}
			else if (mousePosition.x >= static_cast<float>(EngineConfig::GetWindowWidth()) - 1.0f)
			{
				mousePosition.x = 2.0f;
				Input::SetMousePosition(mousePosition);
				io.MousePos = ImVec2(mousePosition.x, mousePosition.y);
				io.MouseDelta = ImVec2(0.0f, 0.0f);
			}

			if (Input::GetMouseButtonUp(SDL_BUTTON_LEFT) || !ImGui::IsWindowFocused())
			{
				draggingBox = false;
			}
		}

		return draggingBox;
	}

	void Component::SetGameObject(GameObject* _gameObject)
	{
		gameObject = _gameObject;
	}
}
