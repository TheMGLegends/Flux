#include "SceneViewCamera.h"

#include <sstream>

#include "Core/Configs/EditorConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Helpers/MathHelpers.h"
#include "Core/Input/Input.h"
#include "Engine/Entities/Components/Camera.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

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

	// INFO: Initial Rotation Setup
	transform.lock()->SetRotation(Quaternion::CreateFromYawPitchRoll(DirectX::XM_PI, 0.0f, 0.0f));

	// INFO: Overwrite default skybox with debug skybox
	camera.lock()->SetMaterialTexture("DebugSkybox");
}

SceneViewCamera::~SceneViewCamera()
{
}

void SceneViewCamera::LateUpdate(float deltaTime)
{
	// INFO: Mouse Mode Switching Logic
	if (Input::GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		EditorConfig::isPanning = true;
		Input::SetMouseMode(true);
	}
	else if (Input::GetMouseButtonUp(SDL_BUTTON_RIGHT))
	{
		EditorConfig::isPanning = false;
		Input::SetMouseMode(false);
	}

	auto cameraRef = camera.lock();
	auto transformRef = transform.lock();

	// INFO: Rotation Logic
	if (Input::GetMouseButton(SDL_BUTTON_RIGHT) && !Input::GetMouseButtonDown(SDL_BUTTON_RIGHT))
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
	float scrollValue = 0.0f;
	if (Input::GetMouseVerticalScroll(scrollValue))
	{
		float oldMovementSpeed = movementSpeed;

		movementSpeed += scrollValue;
		movementSpeed = MathHelpers::Clamp(movementSpeed, minMovementSpeed, maxMovementSpeed);

		if (oldMovementSpeed != movementSpeed)
		{
			std::stringstream ss;
			ss << std::fixed << std::setprecision(0) << movementSpeed;
			std::string movementSpeedStr = ss.str();

			Debug::Log("Movement Speed: " + movementSpeedStr);
		}
	}
}
