#include "EngineRuntime.h"

#include "Core/Configs/RuntimeConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Time/Time.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Physics/Physics.h"
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
}

void EngineRuntime::Release()
{
	scene.reset();

	Audio::Release();
	Physics::Release();
}

bool EngineRuntime::PreInitialise()
{
	// TODO: Pre-Initialisation Logic (Systems)

	// INFO: Initialise the Physics System
	if (!Physics::Initialise())
	{
		Debug::LogError("EngineRuntime::PreInitialise() - Failed to initialise Physics System");
		return false;
	}

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

void EngineRuntime::Start()
{
	scene->Start();
}

void EngineRuntime::Update(float deltaTime)
{
	scene->Update(deltaTime);
}

void EngineRuntime::FixedUpate(float fixedDeltaTime)
{
	while (Time::PerformPhysicsUpdate())
	{
		scene->GetPhysicsScene().simulate(fixedDeltaTime);
		scene->FixedUpdate(fixedDeltaTime);
		scene->GetPhysicsScene().fetchResults(true);
		Time::DecrementAccumulator();
	}
}

void EngineRuntime::LateUpdate(float deltaTime)
{
	scene->LateUpdate(deltaTime);

	// INFO: Update Audio System and current listener if in play mode
	if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
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
}
