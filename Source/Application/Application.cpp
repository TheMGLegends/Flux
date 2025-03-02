#include "Application.h"

#include <SDL3/SDL.h>

#include "../Core/Globals.h"
#include "../Core/Debug/Debug.h"
#include "../Core/Input/Input.h"
#include "../Core/Time/Time.h"

// TODO: TESTING INCLUDES
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include <assimp/postprocess.h>
#include "../Core/Renderer/Model.h"

Application::Application() : editorRuntime(eventDispatcher), engineRuntime(eventDispatcher), window(nullptr), isRunning(false)
{
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		Debug::LogError("Application::Application() - Failed to initialise SDL Video Subsystem");
	}

	// INFO: Window Creation
	window = SDL_CreateWindow("Flux Engine", EngineSettings::WINDOW_WIDTH, EngineSettings::WINDOW_HEIGHT, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		Debug::LogError("Application::Application() - Failed to create SDL Window");
	}

	if (!Input::PreInitialise())
	{
		Debug::LogError("Application::Application() - Failed to pre-initialise Input System");
	}

	if (!Input::Initialise(window))
	{
		Debug::LogError("Application::Application() - Failed to initialise Input System");
	}

	// TODO: Get viewport information from scene view panel
	if (FAILED(renderer.Initialise(GetWindowHandle(), /*TODO: Temporary*/Viewport(0.0f, 0.0f, 1920.0f, 1080.0f, 0.0f, 1.0f))))
	{
		Debug::LogError("Application::Application() - Failed to initialise Renderer");
	}

	editorRuntime.PreInitialise();
	engineRuntime.PreInitialise();

	editorRuntime.Initialise();
	engineRuntime.Initialise();

	// INFO: Setup Events to Listen For
	eventDispatcher.AddListener(EventType::Quit, this);

	// TODO: TESTING CODE
	Assimp::Importer importer;

	const aiScene* yep = importer.ReadFile("Assets/Models/Cube.obj", aiProcessPreset_TargetRealtime_MaxQuality);

	if (yep)
	{
		Model cube;
		cube.Initialise(renderer.GetDevice(), renderer.GetDeviceContext(), yep, "Cube");
	}

	isRunning = true;
}

Application::~Application()
{
	editorRuntime.Release();
	engineRuntime.Release();

	Input::Release();
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Application::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
	switch (eventType)
	{
	case EventType::Quit:
		isRunning = false;
		break;
	case EventType::None:
	default:
		break;
	}
}

void Application::Run()
{
	while (isRunning)
	{
		Time::Tick();
		Input::Update(eventDispatcher);

		editorRuntime.Update(Time::DeltaTime());

		if (RuntimeState::HasEnteredPlayMode())
		{
			engineRuntime.Start();
			RuntimeState::PlayModeEntered();
		}

		if (RuntimeState::IsInPlayMode())
		{
			engineRuntime.FixedUpate(TimeSettings::FIXED_DELTA_TIME);
			engineRuntime.Update(Time::DeltaTime());
		}

		//renderer.RenderFrame(engineRuntime.GetActiveScene());
		editorRuntime.RenderGUI();

		eventDispatcher.ProcessEvents();
	}
}

HWND Application::GetWindowHandle() const
{
	if (!window)
	{
		Debug::LogError("Application::GetWindowHandle() - Window is null");
		return HWND();
	}

	return (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
}
