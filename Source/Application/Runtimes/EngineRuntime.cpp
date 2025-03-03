#include "EngineRuntime.h"

#include "../../Core/Time/Time.h"
#include "../../Engine/Core/Scene/Scene.h"

EngineRuntime::EngineRuntime(EventDispatcher& _eventDispatcher) : Runtime(_eventDispatcher)
{
}

EngineRuntime::~EngineRuntime()
{
}

bool EngineRuntime::PreInitialise()
{
	// TODO: Pre-Initialisation Logic (Systems)

	return true; // INFO: Pre-Initialisation Successful
}

bool EngineRuntime::Initialise()
{
	// TODO: Initialisation Logic

	return true; // INFO: Initialisation Successful
}

void EngineRuntime::Update(float deltaTime)
{
	if (scene.get())
		scene->Update(deltaTime);

	// TODO: Update All Components

	if (scene.get())
		scene->LateUpdate(deltaTime);
}

void EngineRuntime::Release()
{
	// TODO: Release Logic
}

void EngineRuntime::Start()
{
	if (scene.get())
		scene->Start();
}

void EngineRuntime::FixedUpate(float fixedDeltaTime)
{
	while (Time::PerformPhysicsUpdate())
	{
		if (scene.get())
			scene->FixedUpdate(fixedDeltaTime);
		// TODO: Simulate Physics Scene
	}
}
