#include "SceneHierarchy.h"

#include <imgui.h>
#include <memory>

#include "Core/GlobalDefines.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace Flux::GlobalDefines;

SceneHierarchy::SceneHierarchy() : scene(SceneContext::GetScene()), selectedGameObject(nullptr)
{
	EventDispatcher::AddListener(EventType::PlayModeExited, this);
}

SceneHierarchy::~SceneHierarchy()
{
}

int SceneHierarchy::Initialise()
{
	return FLUX_SUCCESS;
}

void SceneHierarchy::Update(float deltaTime)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 5.0f));

	if (ImGui::Begin("Scene Hierarchy"))
	{
		ImVec2 windowSize = ImGui::GetWindowSize();

		ImGui::SetCursorPosX((windowSize.x - ImGui::CalcTextSize(scene.GetSceneName().c_str()).x) * 0.5f);
		ImGui::Text(scene.GetSceneName().c_str());
		ImGui::PopStyleVar();

		ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuterH;
		if (ImGui::BeginTable("##SceneObjects", 1, tableFlags, ImVec2(windowSize.x, windowSize.y * 0.8f), windowSize.x))
		{
			for (size_t i = 0; i < scene.gameObjects.size(); i++)
			{
				std::unique_ptr<GameObject>& gameObject = scene.gameObjects[i];

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Selectable(("  " + gameObject->GetName()).c_str(), selectedGameObject == gameObject.get()))
				{
					selectedGameObject = gameObject.get();
				}
			}

			ImGui::EndTable();
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

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
			}

			// INFO: Go through each reflected game object type and add a menu item for each
			for (auto& it : GameObject::GetGameObjectTypes())
			{
				if (ImGui::MenuItem(it.first.c_str()))
				{
					scene.gameObjects.emplace_back(GameObject::CreateGameObject(it.first));
				}
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}

void SceneHierarchy::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
	if (eventType == EventType::PlayModeExited) { selectedGameObject = nullptr; }
}

GameObject* SceneHierarchy::GetSelectedGameObject() const
{
	return selectedGameObject;
}
