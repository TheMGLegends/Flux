#include "SceneView.h"

#include <ImGuizmo.h>

#include "SceneHierarchy.h"

#include "Core/GlobalDefines.h"

#include "Core/Configs/EditorConfig.h"
#include "Core/Configs/EngineConfig.h"
#include "Core/Configs/RuntimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Helpers/MathHelpers.h"
#include "Core/Input/Input.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Renderer/Renderer.h"

#include "Engine/Audio/Audio.h"
#include "Engine/Scene/SceneContext.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	using namespace EditorConfig;
	using namespace GlobalDefines;

	SceneView::SceneView(Renderer& _renderer, SceneHierarchy* _sceneHierarchy) : renderer(_renderer), sceneHierarchy(_sceneHierarchy)
	{
	}

	SceneView::~SceneView()
	{
	}

	int SceneView::Initialise()
	{
		if (!sceneHierarchy)
		{
			Debug::LogError("SceneView::Initialise() - SceneHierarchy is null");
			return FLUX_FAILURE;
		}

		return FLUX_SUCCESS;
	}

	void SceneView::Update(float deltaTime)
	{
		if (ImGui::Begin("Scene View"))
		{
			EditorConfig::isMouseOverSceneView = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

			// INFO: Only allow Transform Mode Switching if Scene View is Focused
			EditorConfig::isSceneViewFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			if (EditorConfig::isSceneViewFocused)
			{
				// INFO: Keys for Transform Modes
				if (!Input::GetMouseButton(SDL_BUTTON_RIGHT) && !ImGuizmo::IsUsing())
				{
					if (Input::GetKeyDown(SDL_SCANCODE_Q)) { EditorConfig::currentTransformOperation = -1; }
					if (Input::GetKeyDown(SDL_SCANCODE_W)) { EditorConfig::currentTransformOperation = ImGuizmo::OPERATION::TRANSLATE; }
					if (Input::GetKeyDown(SDL_SCANCODE_E)) { EditorConfig::currentTransformOperation = ImGuizmo::OPERATION::ROTATE; }
					if (Input::GetKeyDown(SDL_SCANCODE_R)) { EditorConfig::currentTransformOperation = ImGuizmo::OPERATION::SCALE; }
					if (Input::GetKeyDown(SDL_SCANCODE_Z)) { EditorConfig::SwitchTransformMode(); }
				}
			}
			// INFO: Add Window Focus if panning with right mouse button whilst hovering over Scene View
			else
			{
				if (Input::GetMouseButtonDown(SDL_BUTTON_RIGHT) && ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
				{
					ImGui::SetWindowFocus();
				}
			}

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
			if (RuntimeConfig::IsInEditorMode())
			{
				GameObject* selectedGameObject = sceneHierarchy != nullptr ? sceneHierarchy->GetSelectedGameObject() : nullptr;

				if (selectedGameObject)
				{
					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();

					ImVec2 windowPos = ImGui::GetWindowPos();
					ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);

					// INFO: Retrieve active camera and camera matrices
					std::shared_ptr<Camera> camera = SceneContext::GetScene().GetCamera().lock();
					DirectX::XMFLOAT4X4 cameraView{};
					DirectX::XMFLOAT4X4 cameraProjection{};
					DirectX::XMStoreFloat4x4(&cameraView, camera->GetViewMatrix());
					DirectX::XMStoreFloat4x4(&cameraProjection, camera->GetProjectionMatrix());

					// INFO: Snapping
					bool isSnapEnabled = Input::GetKey(SDL_SCANCODE_LCTRL);

					// INFO: Populate Snap Values
					float snapValues[3]{};

					switch (EditorConfig::currentTransformOperation)
					{
					case ImGuizmo::OPERATION::TRANSLATE:
					{
						snapValues[0] = EditorConfig::translationSnapValue;
						snapValues[1] = EditorConfig::translationSnapValue;
						snapValues[2] = EditorConfig::translationSnapValue;
						break;
					}
					case ImGuizmo::OPERATION::ROTATE:
					{
						snapValues[0] = EditorConfig::rotationSnapValue;
						snapValues[1] = EditorConfig::rotationSnapValue;
						snapValues[2] = EditorConfig::rotationSnapValue;
						break;
					}
					case ImGuizmo::OPERATION::SCALE:
					{
						snapValues[0] = EditorConfig::scaleSnapValue;
						snapValues[1] = EditorConfig::scaleSnapValue;
						snapValues[2] = EditorConfig::scaleSnapValue;
						break;
					}
					default:
						break;
					}

					// INFO: Retrieve selected game object's transform
					std::shared_ptr<Transform> transform = selectedGameObject->transform.lock();
					DirectX::XMFLOAT4X4 transformMatrix{};
					DirectX::XMStoreFloat4x4(&transformMatrix, transform->GetWorldMatrix());

					if (EditorConfig::currentTransformOperation != -1)
					{
						bool overrideMode = false;

						if (EditorConfig::currentTransformOperation == ImGuizmo::OPERATION::SCALE && EditorConfig::transformMode == ImGuizmo::MODE::WORLD)
						{
							// INFO: ImGuizmo does not support scaling in world mode
							overrideMode = true;
						}

						ImGuizmo::Manipulate(&cameraView.m[0][0], &cameraProjection.m[0][0],
							(ImGuizmo::OPERATION)EditorConfig::currentTransformOperation,
							overrideMode ? ImGuizmo::MODE::LOCAL : EditorConfig::transformMode,
							&transformMatrix.m[0][0], nullptr, isSnapEnabled ? snapValues : nullptr);
					}

					if (ImGuizmo::IsUsing())
					{
						DirectX::XMVECTOR position{};
						DirectX::XMVECTOR rotation{};
						DirectX::XMVECTOR scale{};

						DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(&transformMatrix);
						DirectX::XMMatrixDecompose(&scale, &rotation, &position, matrix);

						Vector3 positionVector;
						DirectX::XMStoreFloat3(&positionVector, position);
						transform->SetPositionEditor(positionVector);

						Vector3 scaleVector;
						DirectX::XMStoreFloat3(&scaleVector, scale);
						transform->SetScale(scaleVector);

						Quaternion rotationQuaternion;
						DirectX::XMStoreFloat4(&rotationQuaternion, rotation);
						transform->SetRotationEditor(rotationQuaternion);
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

					switch (EditorConfig::currentTransformOperation)
					{
					case -1: // INFO: Pan Operation (Not Supported by ImGuizmo)
						panTexture = (ImTextureID)AssetHandler::GetTexture("PanButtonSelected");
						break;
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

					// INFO: Pan Button
					if (ImGui::ImageButton("PanButton", panTexture, { 40.0f, 40.0f }))
					{
						// TODO: Logic for Pan Button

						EditorConfig::currentTransformOperation = -1;
					}

					ImGui::SameLine();

					// INFO: Translation Button
					if (ImGui::ImageButton("TranslationButton", translationTexture, { 40.0f, 40.0f }))
					{
						// TODO: Logic for Translation Button

						EditorConfig::currentTransformOperation = ImGuizmo::OPERATION::TRANSLATE;
					}

					ImGui::SameLine();

					// INFO: Rotation Button
					if (ImGui::ImageButton("RotationButton", rotationTexture, { 40.0f, 40.0f }))
					{
						// TODO: Logic for Rotation Button

						EditorConfig::currentTransformOperation = ImGuizmo::OPERATION::ROTATE;
					}

					ImGui::SameLine();

					// INFO: Scale Button
					if (ImGui::ImageButton("ScaleButton", scaleTexture, { 40.0f, 40.0f }))
					{
						// TODO: Logic for Scale Button

						EditorConfig::currentTransformOperation = ImGuizmo::OPERATION::SCALE;
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
}
