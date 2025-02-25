#include "Application.h"

#include <SDL3/SDL.h>

#include "../Core/Globals.h"
#include "../Core/Input/Input.h"
#include "../Core/Time/Time.h"

#include <iostream> // TODO: TESTING

Application::Application() : isRunning(false)
{
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		// TODO: Logging System Log Error Message
	}

	// INFO: Window Creation
	window = SDL_CreateWindow("Flux Engine", EngineSettings::WINDOW_WIDTH, EngineSettings::WINDOW_HEIGHT, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		// TODO: Logging System Log Error Message
	}

	if (!Input::PreInitialise())
	{
		// TODO: Logging System Log Error Message
	}

	if (!Input::Initialise(window))
	{
		// TODO: Logging System Log Error Message
	}

	// TODO: EventDispatcher

	//editorRuntime = EditorRuntime(eventDispatcher);
	//engineRuntime = EngineRuntime(eventDispatcher);

	//editorRuntime.PreInitialise();
	//engineRuntime.PreInitialise();

	// TODO: Renderer (Need to pass over hWnd)

	//editorRuntime.Initialise();
	//engineRuntime.Initialise();

	isRunning = true;
}

Application::~Application()
{
	//editorRuntime.Release();
	//engineRuntime.Release();

	Input::Release();
	SDL_Quit();
}

void Application::Run()
{
	while (isRunning)
	{
		Time::Tick();
		Input::Update();

		//DirectX::SimpleMath::Vector2 joystickAxes = Input::GetJoystickAxes(GamepadJoystick::Left);
		//std::cout << "Left Joystick: " << joystickAxes.x << ", " << joystickAxes.y << std::endl;

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
