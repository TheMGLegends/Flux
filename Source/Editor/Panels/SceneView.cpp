#include "SceneView.h"

#include <ImGuizmo.h>

#include "Core/GlobalDefines.h"
#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/EngineConfig.h"
#include "Core/Configs/RuntimeConfig.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Helpers/MathHelpers.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/Renderer.h"
#include "Engine/Audio/Audio.h"

// TODO: TESTING
#include "Core/Debug/Debug.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace DirectX::SimpleMath;
using namespace Flux::EditorConfig;
using namespace Flux::GlobalDefines;

SceneView::SceneView(Renderer& _renderer) : renderer(_renderer)
{
}

SceneView::~SceneView()
{
}

int SceneView::Initialise()
{
	return FLUX_SUCCESS;
}

void SceneView::Update(float deltaTime)
{
	if (ImGui::Begin("Scene View"))
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 sceneViewSize = ImGui::GetWindowSize();
		MaintainAspectRatio(sceneViewSize);

		// INFO: Centre Scene View
		ImGui::SetCursorPos({ (windowSize.x - sceneViewSize.x) * 0.5f, (windowSize.y - sceneViewSize.y) * 0.5f });

		ImGui::Image((ImTextureID)renderer.GetRenderTextureShaderResourceView(), sceneViewSize);

		// INFO: Check and Update Scene View Size if Necessary
		if (EditorConfig::sceneViewWidth != sceneViewSize.x || EditorConfig::sceneViewHeight != sceneViewSize.y)
		{
			EditorConfig::sceneViewWidth = sceneViewSize.x;
			EditorConfig::sceneViewHeight = sceneViewSize.y;
			EventDispatcher::QueueEvent(EventType::SceneViewResized, nullptr);
		}

		// INFO: Gizmos
		if (RuntimeConfig::IsInEditorMode() || RuntimeConfig::IsPaused())
		{
			GameObject* selectedGameObject = SceneContext::GetScene().GetSelectedGameObject();

			if (selectedGameObject)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();

				ImVec2 windowPos = ImGui::GetWindowPos();
				ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

				// INFO: Retrieve active camera and camera matrices
				auto camera = SceneContext::GetScene().GetCamera().lock();
				DirectX::XMFLOAT4X4 cameraView{};
				DirectX::XMFLOAT4X4 cameraProjection{};
				DirectX::XMStoreFloat4x4(&cameraView, camera->GetViewMatrix());
				DirectX::XMStoreFloat4x4(&cameraProjection, camera->GetProjectionMatrix());


				// INFO: Retrieve selected game object's transform
				auto transform = selectedGameObject->transform.lock();
				DirectX::XMFLOAT4X4 transformMatrix{};
				DirectX::XMStoreFloat4x4(&transformMatrix, transform->GetWorldMatrix());

				ImGuizmo::Manipulate(&cameraView.m[0][0], &cameraProjection.m[0][0], EditorConfig::transformOperation, EditorConfig::transformMode, &transformMatrix.m[0][0]);

				if (ImGuizmo::IsUsing())
				{
					transform->SetPositionEditor(Vector3(transformMatrix._41, transformMatrix._42, transformMatrix._43));
				}
			}
		}

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

		// INFO: Remove Background Colour for Buttons
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		// INFO: Gizmo Selection Buttons
		ImGui::SetCursorPos({ (windowSize.x - sceneViewSize.x) * 0.5f, (windowSize.y - sceneViewSize.y) * 0.5f });

		if (ImGui::BeginChild("DummyChild", { 0, 0 }, true, windowFlags))
		{
			// INFO: Panel Background Styles
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.172f, 0.169f, 0.169f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 2.5f, 2.5f });
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.0f, 0.0f });

			windowFlags &= ~ImGuiWindowFlags_NoBackground;

			// INFO: Gizmo Selector Panel
			if (ImGui::BeginChild("GizmoSelectors", { 196, 50 }, true, windowFlags))
			{
				ImTextureID panTexture = (ImTextureID)AssetHandler::GetTexture("PanButton");
				ImTextureID translationTexture = (ImTextureID)AssetHandler::GetTexture("TranslationButton");
				ImTextureID rotationTexture = (ImTextureID)AssetHandler::GetTexture("RotationButton");
				ImTextureID scaleTexture = (ImTextureID)AssetHandler::GetTexture("ScaleButton");

				if (EditorConfig::isPanning)
				{
					panTexture = (ImTextureID)AssetHandler::GetTexture("PanButtonSelected");
				}
				else
				{
					switch (EditorConfig::transformOperation)
					{
					case ImGuizmo::OPERATION::TRANSLATE:
						translationTexture = (ImTextureID)AssetHandler::GetTexture("TranslationButtonSelected");
						break;
					case ImGuizmo::OPERATION::ROTATE:
						rotationTexture = (ImTextureID)AssetHandler::GetTexture("RotationButtonSelected");
						break;
					case ImGuizmo::OPERATION::SCALE:
						scaleTexture = (ImTextureID)AssetHandler::GetTexture("ScaleButtonSelected");
						break;
					default:
						break;
					}
				}

				// INFO: Pan Button
				if (ImGui::ImageButton("PanButton", panTexture, { 40.0f, 40.0f }))
				{
					// TODO: Logic for Pan Button

					EditorConfig::isPanning = true;
				}

				ImGui::SameLine();

				// INFO: Translation Button
				if (ImGui::ImageButton("TranslationButton", translationTexture, { 40.0f, 40.0f }))
				{
					// TODO: Logic for Translation Button

					EditorConfig::transformOperation = ImGuizmo::OPERATION::TRANSLATE;
				}

				ImGui::SameLine();

				// INFO: Rotation Button
				if (ImGui::ImageButton("RotationButton", rotationTexture, { 40.0f, 40.0f }))
				{
					// TODO: Logic for Rotation Button

					EditorConfig::transformOperation = ImGuizmo::OPERATION::ROTATE;
				}

				ImGui::SameLine();

				// INFO: Scale Button
				if (ImGui::ImageButton("ScaleButton", scaleTexture, { 40.0f, 40.0f }))
				{
					// TODO: Logic for Scale Button

					EditorConfig::transformOperation = ImGuizmo::OPERATION::SCALE;
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleVar(3);
				ImGui::EndChild();
			}

			ImGui::EndChild();
		}

		windowFlags |= ImGuiWindowFlags_NoBackground;
		ImTextureID buttonTexture = 0;

		// INFO: Play Button
		ImGui::SetCursorPos({ (windowSize.x / 2.0f) - 55.0f, (windowSize.y - sceneViewSize.y) * 0.5f });

		if (ImGui::BeginChild("PlayButton", { 0.0f, 0.0f }, true, windowFlags))
		{
			// INFO: Determine Play Button Texture
			buttonTexture = (ImTextureID)(RuntimeConfig::IsInEditorMode() ? AssetHandler::GetTexture("PlayButtonEditorMode") : AssetHandler::GetTexture("PlayButtonPlayMode"));

			// INFO: If the Play Button is Pressed
			if (ImGui::ImageButton("PlayButton", buttonTexture, { 50.0f, 50.0f }))
			{
				// INFO: Logic for entering Play Mode
				if (RuntimeConfig::IsInEditorMode())
				{
					RuntimeConfig::SetMode(RuntimeConfig::Mode::Play);
				}
				// INFO: Logic for exiting Play Mode
				else
				{
					RuntimeConfig::SetMode(RuntimeConfig::Mode::Editor);
					Audio::StopAllSounds();

					// INFO: Unpause the game if it was paused
					if (RuntimeConfig::IsPaused())
						RuntimeConfig::TogglePause();

					// TODO: Event to Reset Scene
					EventDispatcher::QueueEvent(EventType::PlayModeExited, nullptr);
				}
			}

			ImGui::EndChild();
		}

		// INFO: Pause Button
		ImGui::SetCursorPos({ (windowSize.x / 2.0f) + 5.0f, (windowSize.y - sceneViewSize.y) * 0.5f });

		if (ImGui::BeginChild("PauseButton", { 0.0f, 0.0f }, true, windowFlags))
		{
			if (RuntimeConfig::IsInEditorMode())
			{
				buttonTexture = (ImTextureID)AssetHandler::GetTexture("PauseButtonUnavailable");
			}
			else
			{
				if (RuntimeConfig::IsPaused())
				{
					buttonTexture = (ImTextureID)AssetHandler::GetTexture("PauseButtonSelected");
				}
				else
				{
					buttonTexture = (ImTextureID)AssetHandler::GetTexture("PauseButtonAvailable");
				}
			}

			if (ImGui::ImageButton("PauseButton", buttonTexture, { 50.0f, 50.0f }))
			{
				if (RuntimeConfig::IsInPlayMode())
				{
					RuntimeConfig::TogglePause();

					// INFO: Pause/Unpause Audio
					if (RuntimeConfig::IsPaused())
					{
						Audio::ControlSounds(true);
					}
					else
					{
						Audio::ControlSounds(false);
					}
				}
			}

			ImGui::EndChild();
		}

		ImGui::PopStyleColor(3);
		ImGui::End();
	}
}

void SceneView::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
}

void SceneView::MaintainAspectRatio(ImVec2& sceneViewSize)
{
	// INFO: Maintain Aspect Ratio
	float adheringWidth = sceneViewSize.y * EngineConfig::ASPECT_RATIO;
	float adheringHeight = sceneViewSize.x * (1.0f / EngineConfig::ASPECT_RATIO);

	if (sceneViewSize.x >= adheringWidth)
	{
		sceneViewSize.x = adheringWidth;
	}
	else
	{
		sceneViewSize.y = adheringHeight;
	}

	// INFO: Adjust for Top Bar and Scroll Bar
	sceneViewSize.x -= 15.0f;
	sceneViewSize.y -= 40.0f;
}
