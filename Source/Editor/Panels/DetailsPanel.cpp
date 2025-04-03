#include "DetailsPanel.h"

#include <imgui.h>
#include <string/imgui_stdlib.h>

#include "SceneHierarchy.h"
#include "Core/GlobalDefines.h"
#include "Core/Debug/Debug.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace Flux::GlobalDefines;

DetailsPanel::DetailsPanel(SceneHierarchy* _sceneHierarchy) : sceneHierarchy(_sceneHierarchy)
{
}

DetailsPanel::~DetailsPanel()
{
}

int DetailsPanel::Initialise()
{
	return FLUX_SUCCESS;
}

void DetailsPanel::Update(float deltaTime)
{
	if (!sceneHierarchy)
	{
		Debug::LogError("DetailsPanel::Update() - Scene Hierarchy is null");
		return;
	}

	GameObject* selectedGameObject = sceneHierarchy->GetSelectedGameObject();

	if (ImGui::Begin("Details Panel"))
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImGui::GetWindowPos();

		if (selectedGameObject)
		{
			// INFO: Active Checkbox
			bool isActive = selectedGameObject->IsActive();

			if (ImGui::Checkbox("##GameObjectActive", &isActive))
			{
				selectedGameObject->SetIsActive(isActive);
			}

			ImGui::SameLine();

			// INFO: Displaying Game Object Name
			float inputWidth = ImGui::CalcTextSize(selectedGameObject->GetName().c_str()).x;
			inputWidth += 9.0f;

			ImGui::SetCursorPosX((windowSize.x - inputWidth) * 0.5f);
			ImGui::SetNextItemWidth(inputWidth);
			ImGui::InputText("##GameObjectName", &selectedGameObject->GetName(), ImGuiInputTextFlags_EnterReturnsTrue);
		}

		ImGui::End();
	}
}
