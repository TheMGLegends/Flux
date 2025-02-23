#include "EngineRuntime.h"

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
	// TODO: Update Logic from GameObjects in Scene

	// TODO: Update All Components
}

void EngineRuntime::Render()
{
	// TODO: Render Logic
}

void EngineRuntime::Release()
{
	// TODO: Release Logic
}

void EngineRuntime::Start()
{
	// TODO: Start Logic
}

void EngineRuntime::FixedUpate(float fixedDeltaTime)
{
	// TODO: Fixed Update Logic
	// While loop until accumulated time is less than fixedDeltaTime
}

void EngineRuntime::ProcessEvents()
{
	// TODO: Process Events Logic
}
