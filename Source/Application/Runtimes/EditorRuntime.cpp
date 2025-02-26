#include "EditorRuntime.h"

EditorRuntime::EditorRuntime(EventDispatcher& _eventDispatcher) : Runtime(_eventDispatcher)
{
}

EditorRuntime::~EditorRuntime()
{
}

bool EditorRuntime::PreInitialise()
{
	// TODO: Pre-Initialisation Logic (Systems)

	return true; // INFO: Pre-Initialisation Successful
}

bool EditorRuntime::Initialise()
{
	// TODO: Initialisation Logic

	return true; // INFO: Initialisation Successful
}

void EditorRuntime::Update(float deltaTime)
{
	// TODO: Update Logic
}

void EditorRuntime::RenderGUI()
{
	// TODO: RenderGUI Logic
}

void EditorRuntime::Release()
{
	// TODO: Release Logic
}
