#include "EngineRuntime.h"

#include "Core/Debug/Debug.h"
#include "Core/Time/Time.h"
#include "Engine/Scene/Scene.h"

using namespace Flux;

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
