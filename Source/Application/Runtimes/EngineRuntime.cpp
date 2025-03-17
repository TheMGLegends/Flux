#include "EngineRuntime.h"

#include "Core/Configs/RuntimeConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Time/Time.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Entities/Components/Camera.h"
#include "Engine/Entities/Components/Transform.h"
#include "Engine/Entities/GameObjects/GameObject.h"

using namespace Flux;
using namespace DirectX::SimpleMath;

EngineRuntime::EngineRuntime()
{
}

EngineRuntime::~EngineRuntime()
{
	// TODO: Cleanup Logic
}

bool EngineRuntime::PreInitialise()
{
	// TODO: Pre-Initialisation Logic (Systems)

	// INFO: Initialise the Audio System
	if (!Audio::Initialise())
	{
		Debug::LogError("EngineRuntime::PreInitialise() - Failed to initialise Audio System");
		return false;
	}

	return true; // INFO: Pre-Initialisation Successful
}

bool EngineRuntime::Initialise()
{
	// TODO: Initialisation Logic

	// INFO: Initialise the scene
	scene = std::make_unique<Scene>();

	if (!scene)
	{
		Debug::LogError("EngineRuntime::Initialise() - Failed to create Scene");
		return false;
	}

	return true; // INFO: Initialisation Successful
}

void EngineRuntime::Update(float deltaTime)
{
	scene->Update(deltaTime);

	// TODO: Update All Components

	scene->LateUpdate(deltaTime);

	// INFO: Update Audio System and current listener if in play mode
	if (RuntimeConfig::IsInPlayMode())
	{
		if (auto camera = scene->GetCamera().lock())
		{
			Vector3 position = camera->GetGameObject()->transform.lock()->GetPosition();
			Vector3 forward = camera->Forward();
			Vector3 up = camera->Up();
			Audio::SetListenerAttributes({ position.x, position.y, position.z }, { 0.0f, 0.0f, 0.0f }, 
										 { forward.x, forward.y, forward.z }, { up.x, up.y, up.z });
		}

		Audio::Update();
	}

	// TODO: TESTING
	if (auto camera = scene->GetCamera().lock())
	{
		Vector3 position = camera->GetGameObject()->transform.lock()->GetPosition();
		Vector3 forward = camera->Forward();
		Vector3 up = camera->Up();
		Audio::SetListenerAttributes({ position.x, position.y, position.z }, { 0.0f, 0.0f, 0.0f }, 
									 { forward.x, forward.y, forward.z }, { up.x, up.y, up.z });
	}

	Audio::Update();
}

void EngineRuntime::Start()
{
	scene->Start();
}

void EngineRuntime::FixedUpate(float fixedDeltaTime)
{
	while (Time::PerformPhysicsUpdate())
	{
		scene->FixedUpdate(fixedDeltaTime);
		// TODO: Simulate Physics Scene
	}
}

void EngineRuntime::Release()
{
	Audio::Release();
}
