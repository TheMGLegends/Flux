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
#include "Engine/Entities/GameObjects/SceneViewCamera.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	using namespace EditorConfig;
	using namespace GlobalDefines;

	SceneView::SceneView(Renderer& _renderer, SceneHierarchy* _sceneHierarchy) : renderer(_renderer), sceneHierarchy(_sceneHierarchy),
																				 panTexture(0), panTextureSelected(0),
																				 translationTexture(0), translationTextureSelected(0),
																				 rotationTexture(0), rotationTextureSelected(0),
																				 scaleTexture(0), scaleTextureSelected(0),
																				 playButtonEditorMode(0), playButtonPlayMode(0),
																				 pauseButtonUnavailable(0), pauseButtonSelected(0), 
																				 pauseButtonAvailable(0), isResizing(false),
																				 runCount(2)
	{
	}

	SceneView::~SceneView() = default;

	int SceneView::Initialise()
	{
		if (!sceneHierarchy)
		{
			Debug::LogError("SceneView::Initialise() - SceneHierarchy is null");
			return FLUX_FAILURE;
		}

		// INFO: Cache Scene View Icons
		panTexture = (ImTextureID)AssetHandler::GetTexture("PanButton");
		panTextureSelected = (ImTextureID)AssetHandler::GetTexture("PanButtonSelected");
		
		translationTexture = (ImTextureID)AssetHandler::GetTexture("TranslationButton");
		translationTextureSelected = (ImTextureID)AssetHandler::GetTexture("TranslationButtonSelected");
		
		rotationTexture = (ImTextureID)AssetHandler::GetTexture("RotationButton");
		rotationTextureSelected = (ImTextureID)AssetHandler::GetTexture("RotationButtonSelected");
		
		scaleTexture = (ImTextureID)AssetHandler::GetTexture("ScaleButton");
		scaleTextureSelected = (ImTextureID)AssetHandler::GetTexture("ScaleButtonSelected");

		playButtonEditorMode = (ImTextureID)AssetHandler::GetTexture("PlayButtonEditorMode");
		playButtonPlayMode = (ImTextureID)AssetHandler::GetTexture("PlayButtonPlayMode");
		pauseButtonUnavailable = (ImTextureID)AssetHandler::GetTexture("PauseButtonUnavailable");
		pauseButtonSelected = (ImTextureID)AssetHandler::GetTexture("PauseButtonSelected");
		pauseButtonAvailable = (ImTextureID)AssetHandler::GetTexture("PauseButtonAvailable");

		return FLUX_SUCCESS;
	}

	void SceneView::Update(float deltaTime)
	{
		if (ImGui::Begin("Scene View"))
		{
			EditorConfig::SetIsMouseOverSceneView(ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows));

			// INFO: Only allow Transform Mode Switching if Scene View is Focused
			EditorConfig::SetIsSceneViewFocused(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows));

			if (EditorConfig::IsSceneViewFocused())
			{
				// INFO: Keys for Transform Modes
				if (!Input::GetMouseButton(SDL_BUTTON_RIGHT) && !ImGuizmo::IsUsing())
				{
					if (Input::GetKeyDown(SDL_SCANCODE_Q)) { EditorConfig::SetCurretTransformOperation(EditorConfig::PAN); }
					if (Input::GetKeyDown(SDL_SCANCODE_W)) { EditorConfig::SetCurretTransformOperation(ImGuizmo::OPERATION::TRANSLATE); }
					if (Input::GetKeyDown(SDL_SCANCODE_E)) { EditorConfig::SetCurretTransformOperation(ImGuizmo::OPERATION::ROTATE); }
					if (Input::GetKeyDown(SDL_SCANCODE_R)) { EditorConfig::SetCurretTransformOperation(ImGuizmo::OPERATION::SCALE); }
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

			const GameObject* selectedGameObject = sceneHierarchy != nullptr ? sceneHierarchy->GetSelectedGameObject() : nullptr;

			// INFO: Teleport to selected GameObject with editor camera
			if (Input::GetKeyDown(SDL_SCANCODE_F) && selectedGameObject && !Input::GetMouseButton(SDL_BUTTON_RIGHT) && !ImGuizmo::IsUsing())
			{
				auto transform = selectedGameObject->transform.lock();
				SceneContext::GetScene().GetEditorCamera()->transform.lock()->SetPosition(transform->GetPosition());
			}

			ImVec2 windowSize = ImGui::GetWindowSize();
			ImVec2 sceneViewSize = ImGui::GetWindowSize();
			MaintainAspectRatio(sceneViewSize);

			// INFO: Centre Scene View
			ImGui::SetCursorPos({ (windowSize.x - sceneViewSize.x) * 0.5f, (windowSize.y - sceneViewSize.y) * 0.5f });

			ImGui::Image((ImTextureID)renderer.GetRenderTextureShaderResourceView(), sceneViewSize);

			// INFO: Check and Update Scene View Size if Necessary
			if (EditorConfig::GetSceneViewWidth() != sceneViewSize.x || EditorConfig::GetSceneViewHeight() != sceneViewSize.y)
			{
				isResizing = true;
			}

			if (isResizing && Input::GetMouseButtonUp(SDL_BUTTON_LEFT) || runCount > 0)
			{
				runCount--;
				isResizing = false;
				EditorConfig::SetSceneViewWidth(sceneViewSize.x);
				EditorConfig::SetSceneViewHeight(sceneViewSize.y);
				EventDispatcher::QueueEvent(EventType::SceneViewResized, nullptr);
			}

			// INFO: Gizmos
			DrawGizmos(windowSize);

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground;

			// INFO: Remove Background Colour for Buttons
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			// INFO: Gizmo Selection Buttons
			DrawGizmoButtons(windowSize, sceneViewSize, windowFlags);

			windowFlags |= ImGuiWindowFlags_NoBackground;

			// INFO: Scene View Buttons
			DrawSceneViewButtons(windowSize, sceneViewSize, windowFlags);

			ImGui::PopStyleColor(3);
			ImGui::End();
		}
	}

	void SceneView::MaintainAspectRatio(ImVec2& sceneViewSize) const
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

	void SceneView::DrawGizmos(const ImVec2& windowSize) const
	{
		if (!RuntimeConfig::IsInEditorMode() && !RuntimeConfig::IsPaused()) { return; }

		const GameObject* selectedGameObject = sceneHierarchy != nullptr ? sceneHierarchy->GetSelectedGameObject() : nullptr;

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
			std::array<float, 3> snapValues{};

			switch (EditorConfig::GetCurrentTransformOperation())
			{
			case ImGuizmo::OPERATION::TRANSLATE:
			{
				float translationSnapValue = EditorConfig::GetTranslationSnapValue();

				snapValues[0] = translationSnapValue;
				snapValues[1] = translationSnapValue;
				snapValues[2] = translationSnapValue;
				break;
			}
			case ImGuizmo::OPERATION::ROTATE:
			{
				float rotationSnapValue = EditorConfig::GetRotationSnapValue();

				snapValues[0] = rotationSnapValue;
				snapValues[1] = rotationSnapValue;
				snapValues[2] = rotationSnapValue;
				break;
			}
			case ImGuizmo::OPERATION::SCALE:
			{
				float scaleSnapValue = EditorConfig::GetScaleSnapValue();

				snapValues[0] = scaleSnapValue;
				snapValues[1] = scaleSnapValue;
				snapValues[2] = scaleSnapValue;
				break;
			}
			default:
				break;
			}

			// INFO: Retrieve selected game object's transform
			std::shared_ptr<Transform> transform = selectedGameObject->transform.lock();
			DirectX::XMFLOAT4X4 transformMatrix{};
			DirectX::XMStoreFloat4x4(&transformMatrix, transform->GetWorldMatrix());

			int transformOperation = EditorConfig::GetCurrentTransformOperation();
			ImGuizmo::MODE transformMode = EditorConfig::GetTransformMode();

			if (transformOperation != EditorConfig::PAN)
			{
				bool overrideMode = false;

				if (transformOperation == ImGuizmo::OPERATION::SCALE && transformMode == ImGuizmo::MODE::WORLD)
				{
					// INFO: ImGuizmo does not support scaling in world mode
					overrideMode = true;
				}

				ImGuizmo::Manipulate(&cameraView.m[0][0], &cameraProjection.m[0][0], (ImGuizmo::OPERATION)transformOperation,
					overrideMode ? ImGuizmo::MODE::LOCAL : transformMode, &transformMatrix.m[0][0], nullptr,
					isSnapEnabled ? &snapValues[0] : nullptr);
			}

			if (ImGuizmo::IsUsing() && RuntimeConfig::IsInEditorMode())
			{
				DirectX::XMVECTOR position{};
				DirectX::XMVECTOR rotation{};
				DirectX::XMVECTOR scale{};

				DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(&transformMatrix);
				DirectX::XMMatrixDecompose(&scale, &rotation, &position, matrix);

				Vector3 positionVector;
				DirectX::XMStoreFloat3(&positionVector, position);
				transform->SetPosition(positionVector);

				Vector3 scaleVector;
				DirectX::XMStoreFloat3(&scaleVector, scale);
				transform->SetScale(scaleVector);

				Quaternion rotationQuaternion;
				DirectX::XMStoreFloat4(&rotationQuaternion, rotation);
				transform->SetRotation(rotationQuaternion);

				EditorConfig::SetSceneNeedsSaving(true);
			}
		}
	}

	void SceneView::DrawGizmoButtons(const ImVec2& windowSize, const ImVec2& sceneViewSize, ImGuiWindowFlags windowFlags) const
	{
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
				int transformOperation = EditorConfig::GetCurrentTransformOperation();

				// INFO: Pan Button
				if (ImGui::ImageButton("PanButton", transformOperation != EditorConfig::PAN ? panTexture : panTextureSelected, { 40.0f, 40.0f }))
				{
					EditorConfig::SetCurretTransformOperation(EditorConfig::PAN);
				}

				ImGui::SameLine();

				// INFO: Translation Button
				if (ImGui::ImageButton("TranslationButton", transformOperation != ImGuizmo::OPERATION::TRANSLATE ? translationTexture : translationTextureSelected, { 40.0f, 40.0f }))
				{
					EditorConfig::SetCurretTransformOperation(ImGuizmo::OPERATION::TRANSLATE);
				}

				ImGui::SameLine();

				// INFO: Rotation Button
				if (ImGui::ImageButton("RotationButton", transformOperation != ImGuizmo::OPERATION::ROTATE ? rotationTexture : rotationTextureSelected, { 40.0f, 40.0f }))
				{
					EditorConfig::SetCurretTransformOperation(ImGuizmo::OPERATION::ROTATE);
				}

				ImGui::SameLine();

				// INFO: Scale Button
				if (ImGui::ImageButton("ScaleButton", transformOperation != ImGuizmo::OPERATION::SCALE ? scaleTexture : scaleTextureSelected, { 40.0f, 40.0f }))
				{
					EditorConfig::SetCurretTransformOperation(ImGuizmo::OPERATION::SCALE);
				}

				ImGui::PopStyleColor();
				ImGui::PopStyleVar(3);
				ImGui::EndChild();
			}

			ImGui::EndChild();
		}
	}

	void SceneView::DrawSceneViewButtons(const ImVec2& windowSize, const ImVec2& sceneViewSize, ImGuiWindowFlags windowFlags) const
	{
		ImTextureID buttonTexture = 0;

		// INFO: Play Button
		ImGui::SetCursorPos({ (windowSize.x / 2.0f) - 55.0f, (windowSize.y - sceneViewSize.y) * 0.5f });

		if (ImGui::BeginChild("PlayButton", { 0.0f, 0.0f }, true, windowFlags))
		{
			// INFO: Determine Play Button Texture
			buttonTexture = RuntimeConfig::IsInEditorMode() ? playButtonEditorMode : playButtonPlayMode;

			// INFO: If the Play Button is Pressed
			if (ImGui::ImageButton("PlayButton", buttonTexture, { 50.0f, 50.0f }))
			{
				// INFO: Logic for entering Play Mode
				if (RuntimeConfig::IsInEditorMode())
				{
					// INFO: Warning that unsaved changes will be lost
					if (EditorConfig::SceneNeedsSaving())
					{
						Debug::LogWarning("Scene has unsaved changes, these will be lost once play mode is exited");
					}

					RuntimeConfig::SetMode(RuntimeConfig::Mode::Play);
					Audio::StopSoundInEditor();
				}
				// INFO: Logic for exiting Play Mode
				else
				{
					RuntimeConfig::SetMode(RuntimeConfig::Mode::Editor);
					Audio::StopAllSounds();

					// INFO: Unpause the game if it was paused
					if (RuntimeConfig::IsPaused())
					{
						RuntimeConfig::TogglePause();
					}

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
				buttonTexture = pauseButtonUnavailable;
			}
			else
			{
				if (RuntimeConfig::IsPaused())
				{
					buttonTexture = pauseButtonSelected;
				}
				else
				{
					buttonTexture = pauseButtonAvailable;
				}
			}

			if (ImGui::ImageButton("PauseButton", buttonTexture, { 50.0f, 50.0f }) && RuntimeConfig::IsInPlayMode())
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

			ImGui::EndChild();
		}
	}
}
