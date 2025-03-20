#include "SceneViewCamera.h"

#include "Core/Debug/Debug.h"
#include "Core/Helpers/MathHelpers.h"
#include "Core/Input/Input.h"
#include "Engine/Entities/Components/Camera.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

SceneViewCamera::SceneViewCamera() : rotationSpeed(0.001f), movementSpeed(10.0f), pitchConstraints(-1.49f, 1.49f)
{
	camera = AddComponent<Camera>(this);

	if (camera.expired())
		Debug::LogError("SceneViewCamera::SceneViewCamera() - Failed to add Camera component to SceneViewCamera");
	else
		camera.lock()->SetDrawFrustum(false);
}

SceneViewCamera::~SceneViewCamera()
{
}

void SceneViewCamera::LateUpdate(float deltaTime)
{
	// INFO: Mouse Mode Switching Logic
	if (Input::GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		Input::SetMouseMode(true);
	}
	else if (Input::GetMouseButtonUp(SDL_BUTTON_RIGHT))
	{
		Input::SetMouseMode(false);
	}

	auto cameraRef = camera.lock();
	auto transformRef = transform.lock();

	// INFO: Rotation Logic
	if (Input::GetMouseButton(SDL_BUTTON_RIGHT) && !Input::GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		Vector2 mouseInput = Input::GetMousePosition();

		Vector3 eulerRotation = cameraRef->GetRotation().ToEuler();

		eulerRotation.y += mouseInput.x * rotationSpeed;
		eulerRotation.x -= mouseInput.y * rotationSpeed;

		eulerRotation.x = MathHelpers::Clamp(eulerRotation.x, pitchConstraints.x, pitchConstraints.y);

		transformRef->SetRotation(Quaternion::CreateFromYawPitchRoll(eulerRotation.y, eulerRotation.x, 0.0f));
		cameraRef->SetRotation(transformRef->GetRotation());
	}

	// INFO: Movement Logic
	if (Input::GetKey(SDL_SCANCODE_W))
		transformRef->Translate(transformRef->Forward() * movementSpeed * deltaTime);

	if (Input::GetKey(SDL_SCANCODE_S))
		transformRef->Translate(-transformRef->Forward() * movementSpeed * deltaTime);

	if (Input::GetKey(SDL_SCANCODE_A))
		transformRef->Translate(transformRef->Right() * movementSpeed * deltaTime);

	if (Input::GetKey(SDL_SCANCODE_D))
		transformRef->Translate(-transformRef->Right() * movementSpeed * deltaTime);
}
