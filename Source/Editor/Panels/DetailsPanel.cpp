#include "DetailsPanel.h"

#include <imgui.h>
#include <magic_enum.hpp>
#include <string/imgui_stdlib.h>

#include "SceneHierarchy.h"
#include "Core/GlobalDefines.h"
#include "Core/Configs/RuntimeConfig.h"
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
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 windowSize = ImGui::GetWindowSize();

		// INFO: Play Mode Overlay
		if (RuntimeConfig::IsInPlayMode() || RuntimeConfig::IsPaused())
		{
			ImGui::GetForegroundDrawList()->AddRectFilled(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), IM_COL32(0, 116, 188, 50));
		}

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

			// INFO: Display Game Object Component Details
			auto& components = selectedGameObject->GetComponents();
			int componentCount = static_cast<int>(components.size());

			for (size_t i = 0; i < components.size(); i++)
			{
				std::shared_ptr<Component>& component = components[i];

				component->DrawDetails();
			}

			// INFO: Add Component Button
			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			ImVec2 buttonSize = ImVec2(150.0f, 50.0f);
			ImGui::SetCursorPosX((windowSize.x - buttonSize.x) * 0.5f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
			if (ImGui::Button("Add Component", buttonSize))
			{
				ImGui::OpenPopup("AddComponentPopup");
			}
			ImGui::PopStyleVar(1);

			if (ImGui::BeginPopup("AddComponentPopup"))
			{
				std::weak_ptr<Component> component;

				if (ImGui::MenuItem("Camera"))
				{
					component = selectedGameObject->AddComponent<Camera>(selectedGameObject);
					ComponentAlreadyExists(componentCount, static_cast<int>(selectedGameObject->GetComponents().size()), component);
				}

				if (ImGui::MenuItem("Physics Body"))
				{
					component = selectedGameObject->AddComponent<PhysicsBody>(selectedGameObject);
					ComponentAlreadyExists(componentCount, static_cast<int>(selectedGameObject->GetComponents().size()), component);
				}

				if (ImGui::MenuItem("Visualizer"))
				{
					component = selectedGameObject->AddComponent<Visualizer>(selectedGameObject);
					ComponentAlreadyExists(componentCount, static_cast<int>(selectedGameObject->GetComponents().size()), component);
				}

				if (ImGui::MenuItem("Box Collider"))
				{
					component = selectedGameObject->AddComponent<BoxCollider>(selectedGameObject);
					ComponentAlreadyExists(componentCount, static_cast<int>(selectedGameObject->GetComponents().size()), component);
				}

				if (ImGui::MenuItem("Sphere Collider"))
				{
					component = selectedGameObject->AddComponent<SphereCollider>(selectedGameObject);
					ComponentAlreadyExists(componentCount, static_cast<int>(selectedGameObject->GetComponents().size()), component);
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}

void DetailsPanel::ComponentAlreadyExists(int oldComponentCount, int newComponentCount, std::weak_ptr<Component> component)
{
	if (oldComponentCount == newComponentCount)
	{
		std::shared_ptr<Component> existingComponent = component.lock();

		if (existingComponent)
		{
			std::string componentType = std::string(magic_enum::enum_name(component.lock()->GetComponentType()));
			std::string gameObjectName = component.lock()->GetGameObject()->GetName();
			Debug::LogWarning("DetailsPanel::ComponentAlreadyExists() - " + componentType + " component already exists on " + gameObjectName);
		}
		else
		{
			Debug::LogWarning("DetailsPanel::ComponentAlreadyExists() - Collider component already exists on the game object");
		}
	}
}
