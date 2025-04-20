#include "SceneHierarchy.h"

#include <imgui.h>
#include <string/imgui_stdlib.h>
#include <ImGuizmo.h>

#include "Core/GlobalDefines.h"

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/GameConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Input/Input.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"

#include "Engine/Scene/SceneContext.h"

namespace Flux
{
	using namespace GlobalDefines;

	SceneHierarchy::SceneHierarchy() : scene(SceneContext::GetScene()), selectedGameObject(nullptr), isRenaming(false), bigFont(nullptr)
	{
	}

	SceneHierarchy::~SceneHierarchy()
	{
		EventDispatcher::RemoveListener(this);
	}

	int SceneHierarchy::Initialise()
	{
		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::PlayModeExited, this)))
		{
			Debug::LogError("Failed to add listener for PlayModeExited event in SceneHierarchy");
			return FLUX_FAILURE;
		}

		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::LoadScene, this)))
		{
			Debug::LogError("Failed to add listener for LoadScene event in SceneHierarchy");
			return FLUX_FAILURE;
		}

		bigFont = AssetHandler::GetImGuiFont("OpenSans-Bold");

		if (!bigFont)
		{
			Debug::LogError("SceneHierarchy::Initialise() - Failed to load big font");
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	void SceneHierarchy::Update(float deltaTime)
	{
		// INFO: Object Deletion Logic
		if (Input::GetKeyDown(SDL_SCANCODE_DELETE) && RuntimeConfig::IsInEditorMode() && selectedGameObject)
		{
			EventDispatcher::Notify(EventType::GameObjectRemoved, std::make_shared<GameObjectRemovedEvent>(selectedGameObject));
			selectedGameObject = nullptr;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		if (ImGui::Begin("Scene Hierarchy"))
		{
			ImVec2 windowPos = ImGui::GetWindowPos();
			ImVec2 windowSize = ImGui::GetWindowSize();

			// INFO: Play Mode Overlay
			if (RuntimeConfig::IsInPlayMode() || RuntimeConfig::IsPaused())
			{
				ImGui::GetForegroundDrawList()->AddRectFilled(windowPos, ImVec2(windowPos.x + windowSize.x, windowPos.y + windowSize.y), IM_COL32(0, 116, 188, 50));
			}

			// INFO: Scene Name
			std::string sceneName = EditorConfig::SceneNeedsSaving() ? scene.GetSceneName() + "*" : scene.GetSceneName();
			ImGui::Dummy(ImVec2(0.0f, 2.5f));
			ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize(sceneName.c_str()).x) * 0.5f);
			ImGui::PushFont(bigFont);
			ImGui::Text(sceneName.c_str());
			ImGui::PopFont();

			// INFO: Game Settings Popup
			GameSettingsPopup();

			// INFO: Search Bar
			ImGui::PushItemWidth(windowSize.x * 0.5f);
			ImGui::SetCursorPosX(windowSize.x * 0.25f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
			ImGui::InputTextWithHint("##SearchBar", "Search...", &searchString, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_AutoSelectAll);
			ImGui::PopStyleVar();

			// INFO: Display Scene Objects
			DisplaySceneObjects(windowSize);

			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			if (RuntimeConfig::IsInPlayMode())
			{
				ImGui::BeginDisabled();
			}

			// INFO: Add GameObject Button
			AddGameObjectButton(windowSize.x);

			if (RuntimeConfig::IsInPlayMode())
			{
				ImGui::EndDisabled();
			}

			ImGui::End();
		}
	}

	void SceneHierarchy::OnNotify(EventType eventType, std::shared_ptr<Event> event)
	{
		if (eventType == EventType::PlayModeExited || eventType == EventType::LoadScene) { selectedGameObject = nullptr; }
	}

	GameObject* SceneHierarchy::GetSelectedGameObject() const
	{
		return selectedGameObject;
	}

	void SceneHierarchy::DisplaySceneObjects(const ImVec2& windowSize)
	{
		ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuterH;
		if (ImGui::BeginTable("##SceneObjects", 1, tableFlags, ImVec2(windowSize.x, windowSize.y * 0.75f), windowSize.x))
		{
			bool selectedGameObjectHovered = false;

			for (const auto& gameObject : scene.gameObjects)
			{
				const std::string& gameObjectName = gameObject->GetName();

				if (!searchString.empty() && gameObjectName.rfind(searchString, 0) == std::string::npos) { continue; }

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);

				// INFO: Unique ID for each GameObject
				if (ImGui::Selectable(("##" + gameObject->GetID()).c_str(), selectedGameObject == gameObject.get()))
				{
					selectedGameObject = gameObject.get();

					// INFO: Default to Translate Operation if we are currently in pan mode
					if (EditorConfig::GetCurrentTransformOperation() == EditorConfig::PAN)
					{
						EditorConfig::SetCurretTransformOperation(ImGuizmo::OPERATION::TRANSLATE);
					}
				}

				if (ImGui::IsItemHovered() && gameObject.get() == selectedGameObject)
				{
					selectedGameObjectHovered = true;

					// INFO: De-select the selected GameObject if right-clicked and isn't renaming
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !isRenaming)
					{
						selectedGameObject = nullptr;
					}
				}

				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && selectedGameObjectHovered)
				{
					isRenaming = true;
				}

				ImGui::SameLine();

				if (isRenaming && selectedGameObject == gameObject.get())
				{
					ImGui::SetKeyboardFocusHere();

					if (ImGui::InputText("##CustomNameInput", &selectedGameObject->GetName(), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						isRenaming = false;
						EditorConfig::SetSceneNeedsSaving(true);
					}
				}
				else
				{
					ImGui::TextUnformatted(gameObjectName.c_str());
				}
			}

			ImGui::EndTable();
		}
	}

	void SceneHierarchy::GameSettingsPopup() const
	{
		// INFO: Ability to Set as Starter Scene if right clicking scene name
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("GameSettings");
		}

		ImGui::SetNextWindowSize(ImVec2(127.0f, 22.0f));
		if (ImGui::BeginPopup("GameSettings"))
		{
			if (ImGui::MenuItem("  Set Starter Scene"))
			{
				GameConfig::SetStarterSceneName(scene.GetSceneName());
				GameConfig::SerializeGameConfig();
			}

			ImGui::EndPopup();
		}

		ImGui::Dummy(ImVec2(0.0f, 2.5f));
		ImGui::PopStyleVar();
	}

	void SceneHierarchy::AddGameObjectButton(float windowWidth)
	{
		ImVec2 buttonSize = ImVec2(150.0f, 50.0f);
		ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
		if (ImGui::Button("Add GameObject", buttonSize))
		{
			ImGui::OpenPopup("AddGameObjectButton");
		}
		ImGui::PopStyleVar(1);

		if (ImGui::BeginPopup("AddGameObjectButton"))
		{
			if (ImGui::MenuItem("Empty GameObject"))
			{
				scene.gameObjects.emplace_back(GameObject::CreateGameObject("GameObject"));
				EditorConfig::SetSceneNeedsSaving(true);
			}

			// INFO: Go through each reflected game object type and add a menu item for each
			for (auto& [name, function] : GameObject::GetGameObjectTypes())
			{
				if (ImGui::MenuItem(name.c_str()))
				{
					scene.gameObjects.emplace_back(GameObject::CreateGameObject(name));
					EditorConfig::SetSceneNeedsSaving(true);
				}
			}

			ImGui::EndPopup();
		}
	}
}
