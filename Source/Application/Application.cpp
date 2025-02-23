#include "Application.h"

#include <SDL3/SDL.h>

#include "../Core/Globals.h"

Application::Application() : isRunning(false)
{
	// TODO: EventDispatcher

	//editorRuntime = EditorRuntime(eventDispatcher);
	//engineRuntime = EngineRuntime(eventDispatcher);

	//editorRuntime.PreInitialise();
	//engineRuntime.PreInitialise();

	// INFO: Window Creation
	window = SDL_CreateWindow("Flux Engine", EngineSettings::WindowWidth, EngineSettings::WindowHeight, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

	// TODO: Renderer (Need to pass over hWnd)

	//editorRuntime.Initialise();
	//engineRuntime.Initialise();

	isRunning = true;
}

Application::~Application()
{
	//editorRuntime.Release();
	//engineRuntime.Release();
}

void Application::Run()
{
	while (isRunning)
	{
		// TODO: Tick Time
		// TODO: Get Input
		
		//editorRuntime.Update(Time::DeltaTime());

		if (RuntimeState::HasEnteredPlayMode())
		{
			//engineRuntime.Start();
			RuntimeState::PlayModeEntered();
		}

		if (RuntimeState::IsInPlayMode())
		{
			//engineRuntime.ProcessEvents();
			//engineRuntime.FixedUpate(Time::FixedDeltaTime()); // TODO: Could move FixedUpate into Update?
			//engineRuntime.Update(Time::DeltaTime());
		}

		//editorRuntime.Render();
		//engineRuntime.Render();

		// TODO: Garbage Cleanup Logic
	}
}
