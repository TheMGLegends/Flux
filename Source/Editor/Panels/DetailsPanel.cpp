#include "DetailsPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <magic_enum.hpp>
#include <string/imgui_stdlib.h>

#include "SceneHierarchy.h"

#include "Core/GlobalDefines.h"

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Renderer/AssetHandler.h"

#include "Engine/Entities/GameObjects/GameObject.h"

namespace Flux
{
	using namespace GlobalDefines;

	DetailsPanel::DetailsPanel(SceneHierarchy* _sceneHierarchy) : sceneHierarchy(_sceneHierarchy), bigFont(nullptr)
	{
	}

	DetailsPanel::~DetailsPanel() = default;

	int DetailsPanel::Initialise()
	{
		bigFont = AssetHandler::GetImGuiFont("OpenSans-Bold");

		if (!bigFont)
		{
			Debug::LogError("SceneHierarchy::Initialise() - Failed to load big font");
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	void DetailsPanel::Update(float deltaTime)
	{
		if (!sceneHierarchy)
		{
			Debug::LogError("DetailsPanel::Update() - Scene Hierarchy is null");
			return;
		}

		if (RuntimeConfig::IsInPlayMode())
		{
			ImGui::BeginDisabled();
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

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
				if (ImGui::Checkbox("##GameObjectActive", &isActive))
				{
					selectedGameObject->SetIsActive(isActive);
					EditorConfig::SetSceneNeedsSaving(true);
				}

				ImGui::SameLine();

				// INFO: Displaying Game Object Name
				ImGui::PushFont(bigFont);
				float inputWidth = ImGui::CalcTextSize(selectedGameObject->GetName().c_str()).x;
				inputWidth += 9.0f;

				ImGui::SetCursorPosX((windowSize.x - inputWidth) * 0.5f);
				ImGui::SetNextItemWidth(inputWidth);
				if (ImGui::InputText("##GameObjectName", &selectedGameObject->GetName(), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					EditorConfig::SetSceneNeedsSaving(true);
				}
				ImGui::PopFont();
				ImGui::PopStyleVar();

				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				// INFO: Display Game Object Component Details
				const std::vector<std::shared_ptr<Component>>& components = selectedGameObject->GetComponents();
				int componentCount = static_cast<int>(components.size());

				ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

				for (const auto& component : components)
				{
					if (component) { component->DrawDetails(); }
				}

				// INFO: Add Component Button
				AddComponentPopup(selectedGameObject, componentCount, windowSize.x);
			}

			ImGui::End();
		}

		if (RuntimeConfig::IsInPlayMode())
		{
			ImGui::EndDisabled();
		}
	}

	void DetailsPanel::AddComponentPopup(GameObject* selectedGameObject, int componentCount, float windowWidth) const
	{
		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImVec2 buttonSize{ 150.0f, 50.0f };
		ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
		if (ImGui::Button("Add Component", buttonSize))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}
		ImGui::PopStyleVar(1);

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			std::weak_ptr<Component> component;
			bool menuItemClicked = false;

			if (ImGui::MenuItem("Camera"))
			{
				component = selectedGameObject->AddComponent<Camera>(selectedGameObject);
				menuItemClicked = true;
			}

			if (ImGui::MenuItem("Physics Body"))
			{
				component = selectedGameObject->AddComponent<PhysicsBody>(selectedGameObject);
				menuItemClicked = true;
			}

			if (ImGui::MenuItem("Visualizer"))
			{
				component = selectedGameObject->AddComponent<Visualizer>(selectedGameObject);
				menuItemClicked = true;
			}

			if (ImGui::MenuItem("Box Collider"))
			{
				component = selectedGameObject->AddComponent<BoxCollider>(selectedGameObject);
				menuItemClicked = true;
			}

			if (ImGui::MenuItem("Sphere Collider"))
			{
				component = selectedGameObject->AddComponent<SphereCollider>(selectedGameObject);
				menuItemClicked = true;
			}

			if (menuItemClicked)
			{
				ComponentAlreadyExists(componentCount, static_cast<int>(selectedGameObject->GetComponents().size()), component);
				EditorConfig::SetSceneNeedsSaving(true);
			}

			ImGui::EndPopup();
		}
	}

	void DetailsPanel::ComponentAlreadyExists(int oldComponentCount, int newComponentCount, std::weak_ptr<Component> component) const
	{
		if (oldComponentCount == newComponentCount)
		{
			std::shared_ptr<Component> existingComponent = component.lock();

			if (existingComponent)
			{
				std::string componentType{ magic_enum::enum_name(component.lock()->GetComponentType()) };
				std::string gameObjectName = component.lock()->GetGameObject()->GetName();
				Debug::LogWarning("DetailsPanel::ComponentAlreadyExists() - " + componentType + " component already exists on " + gameObjectName);
			}
			else
			{
				Debug::LogWarning("DetailsPanel::ComponentAlreadyExists() - Collider component already exists on the game object");
			}
		}
	}
}
