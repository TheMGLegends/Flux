#include "SceneViewCamera.h"

#include "Core/Configs/EditorConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Helpers/MathHelpers.h"
#include "Core/Input/Input.h"

#include "Engine/Entities/Components/Camera.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	SceneViewCamera::SceneViewCamera() : rotationSpeed(0.001f), movementSpeed(10.0f), minMovementSpeed(1.0f),
										 maxMovementSpeed(50.0f), pitchConstraints(-1.49f, 1.49f)
	{
		camera = AddComponent<Camera>(this);

		if (camera.expired())
		{
			Debug::LogError("SceneViewCamera::SceneViewCamera() - Failed to add Camera component to SceneViewCamera");
		}
		else
		{
			camera.lock()->SetDrawFrustum(false);
		}
	}

	SceneViewCamera::~SceneViewCamera() = default;

	void SceneViewCamera::LateUpdate(float deltaTime)
	{
		if (!EditorConfig::IsMouseOverSceneView()) { return; }

		// INFO: Mouse Mode Switching Logic
		if (Input::GetMouseButtonDown(SDL_BUTTON_RIGHT))
		{
			EditorConfig::StorePreviousTransformOperation();
			EditorConfig::SetCurretTransformOperation(EditorConfig::PAN);
			Input::SetMouseMode(true);
		}
		else if (Input::GetMouseButtonUp(SDL_BUTTON_RIGHT))
		{
			EditorConfig::RevertToPreviousTransformOperation();
			Input::SetMouseMode(false);
		}

		std::shared_ptr<Camera> cameraRef = camera.lock();
		std::shared_ptr<Transform> transformRef = transform.lock();

		// INFO: Rotation Logic
		if (Input::GetMouseButton(SDL_BUTTON_RIGHT) && !Input::GetMouseButtonDown(SDL_BUTTON_RIGHT) && Input::IsMouseRelative())
		{
			Vector2 mouseInput = Input::GetMousePosition();
			Vector3 eulerRotation = transformRef->GetRotation().ToEuler();

			eulerRotation.y += mouseInput.x * rotationSpeed;
			eulerRotation.x -= mouseInput.y * rotationSpeed;

			eulerRotation.x = MathHelpers::Clamp(eulerRotation.x, pitchConstraints.x, pitchConstraints.y);

			transformRef->SetRotation(Quaternion::CreateFromYawPitchRoll(eulerRotation.y, eulerRotation.x, 0.0f));
		}

		// INFO: Movement Logic Can Only Happen if RMB is held
		if (Input::GetMouseButton(SDL_BUTTON_RIGHT))
		{
			// INFO: Movement Logic
			if (Input::GetKey(SDL_SCANCODE_W))
			{
				transformRef->Translate(transformRef->Forward() * movementSpeed * deltaTime);
			}

			if (Input::GetKey(SDL_SCANCODE_S))
			{
				transformRef->Translate(-transformRef->Forward() * movementSpeed * deltaTime);
			}

			if (Input::GetKey(SDL_SCANCODE_A))
			{
				transformRef->Translate(transformRef->Right() * movementSpeed * deltaTime);
			}

			if (Input::GetKey(SDL_SCANCODE_D))
			{
				transformRef->Translate(-transformRef->Right() * movementSpeed * deltaTime);
			}

			// INFO: Up & Down Movement Logic (World Space)
			if (Input::GetKey(SDL_SCANCODE_E))
			{
				transformRef->Translate(Vector3::Up * movementSpeed * deltaTime);
			}

			if (Input::GetKey(SDL_SCANCODE_Q))
			{
				transformRef->Translate(-Vector3::Up * movementSpeed * deltaTime);
			}
		}

		// INFO: Movement Speed Adjustment Logic
		if (EditorConfig::IsSceneViewFocused())
		{
			float scrollValue = 0.0f;
			if (Input::GetMouseVerticalScroll(scrollValue))
			{
				float oldMovementSpeed = movementSpeed;

				movementSpeed += scrollValue;
				movementSpeed = MathHelpers::Clamp(movementSpeed, minMovementSpeed, maxMovementSpeed);

				if (oldMovementSpeed != movementSpeed)
				{
					std::string movementSpeedString = std::format("Movement Speed: {:.0f}", movementSpeed);
					Debug::Log(movementSpeedString);
				}
			}
		}
	}

	std::weak_ptr<Camera> SceneViewCamera::GetCamera() const
	{
		return camera;
	}
}
