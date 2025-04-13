#include "SceneHierarchy.h"

#include <imgui.h>
#include <string/imgui_stdlib.h>
#include <ImGuizmo.h>

#include "Core/GlobalDefines.h"

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Input/Input.h"
#include "Core/EventSystem/Events/GameObjectRemovedEvent.h"

#include "Engine/Scene/SceneContext.h"

namespace Flux
{
	using namespace GlobalDefines;

	SceneHierarchy::SceneHierarchy() : scene(SceneContext::GetScene()), selectedGameObject(nullptr), isRenaming(false)
	{
	}

	SceneHierarchy::~SceneHierarchy()
	{
	}

	int SceneHierarchy::Initialise()
	{
		if (IS_FAILURE(EventDispatcher::AddListener(EventType::PlayModeExited, this)))
		{
			Debug::LogError("Failed to add listener for PlayModeExited event in SceneHierarchy");
			return FLUX_FAILURE;
		}

		if (IS_FAILURE(EventDispatcher::AddListener(EventType::LoadScene, this)))
		{
			Debug::LogError("Failed to add listener for LoadScene event in SceneHierarchy");
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
			std::string sceneName = EditorConfig::sceneNeedsSaving ? scene.GetSceneName() + "*" : scene.GetSceneName();
			ImGui::Dummy(ImVec2(0.0f, 2.5f));
			ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize(sceneName.c_str()).x) * 0.5f);
			ImGui::Text(sceneName.c_str());
			ImGui::Dummy(ImVec2(0.0f, 2.5f));
			ImGui::PopStyleVar();

			// INFO: Search Bar
			ImGui::PushItemWidth(windowSize.x * 0.5f);
			ImGui::SetCursorPosX((windowSize.x * 0.25f));
			ImGui::InputTextWithHint("##SearchBar", "Search...", &searchString, ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_AutoSelectAll);

			ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuterH;
			if (ImGui::BeginTable("##SceneObjects", 1, tableFlags, ImVec2(windowSize.x, windowSize.y * 0.75f), windowSize.x))
			{
				bool selectedGameObjectHovered = false;

				for (size_t i = 0; i < scene.gameObjects.size(); i++)
				{
					std::unique_ptr<GameObject>& gameObject = scene.gameObjects[i];

					const std::string& gameObjectName = gameObject->GetName();

					if (searchString.empty() || gameObjectName.rfind(searchString, 0) != std::string::npos)
					{
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);

						// INFO: Unique ID for each GameObject
						if (ImGui::Selectable(("##" + gameObject->GetID()).c_str(), selectedGameObject == gameObject.get()))
						{
							selectedGameObject = gameObject.get();

							// INFO: Default to Translate Operation if we are currently in pan mode
							if (EditorConfig::currentTransformOperation == -1)
							{
								EditorConfig::currentTransformOperation = ImGuizmo::OPERATION::TRANSLATE;
							}
						}

						if (ImGui::IsItemHovered() && gameObject.get() == selectedGameObject)
						{
							selectedGameObjectHovered = true;
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
								EditorConfig::sceneNeedsSaving = true;
							}
						}
						else
						{
							ImGui::TextUnformatted(gameObjectName.c_str());
						}
					}
				}

				ImGui::EndTable();
			}

			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			if (RuntimeConfig::IsInPlayMode())
			{
				ImGui::BeginDisabled();
			}

			ImVec2 buttonSize = ImVec2(150.0f, 50.0f);
			ImGui::SetCursorPosX((windowSize.x - buttonSize.x) * 0.5f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
			if (ImGui::Button("Add GameObject", buttonSize))
			{
				ImGui::OpenPopup("AddGameObjectPopup");
			}
			ImGui::PopStyleVar(1);

			if (ImGui::BeginPopup("AddGameObjectPopup"))
			{
				if (ImGui::MenuItem("Empty GameObject"))
				{
					scene.gameObjects.emplace_back(GameObject::CreateGameObject("GameObject"));
					EditorConfig::sceneNeedsSaving = true;
				}

				// INFO: Go through each reflected game object type and add a menu item for each
				for (auto& it : GameObject::GetGameObjectTypes())
				{
					if (ImGui::MenuItem(it.first.c_str()))
					{
						scene.gameObjects.emplace_back(GameObject::CreateGameObject(it.first));
						EditorConfig::sceneNeedsSaving = true;
					}
				}

				ImGui::EndPopup();
			}

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
}
