#include "Application.h"

#include <SDL3/SDL.h>

#include "Core/Configs/EngineConfig.h"
#include "Core/Configs/RuntimeConfig.h"
#include "Core/Configs/TimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Input/Input.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Time/Time.h"

// TODO: TESTING
#include "Engine/Audio/Audio.h"

using namespace Flux;

Application::Application() : window(nullptr), isRunning(false)
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		Debug::LogError("Application::Application() - Failed to initialise SDL Systems");
	}

	// INFO: Window Creation
	window = SDL_CreateWindow("Flux Engine", EngineConfig::windowWidth, EngineConfig::windowHeight, SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		Debug::LogError("Application::Application() - Failed to create SDL Window");
	}

	if (!Input::Initialise(window))
	{
		Debug::LogError("Application::Application() - Failed to initialise Input System");
	}

	// TODO: Get viewport information from scene view panel
	if (FAILED(renderer.Initialise(GetWindowHandle(), /*TODO: Temporary*/Viewport(0.0f, 0.0f, (float)EngineConfig::windowWidth, (float)EngineConfig::windowHeight, 0.0f, 1.0f))))
	{
		Debug::LogError("Application::Application() - Failed to initialise Renderer");
	}

	if (FAILED(AssetHandler::Initialise(renderer.GetDevice(), renderer.GetDeviceContext())))
	{
		Debug::LogError("Application::Application() - Failed to initialise Asset Handler");
	}

	editorRuntime.PreInitialise();
	engineRuntime.PreInitialise();

	editorRuntime.Initialise();
	engineRuntime.Initialise();

	// INFO: Setup Events to Listen For
	EventDispatcher::AddListener(EventType::Quit, this);

	// TODO: Testing
	Audio::PlaySound3D("testSound", FMOD_VECTOR{ 0.0f, 0.0f, 10.0f }, 1.0f, 500.0f, true);

	isRunning = true;
}

Application::~Application()
{
	engineRuntime.Release();
	Input::Release();

	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Application::OnNotify(EventType eventType, std::shared_ptr<Event> event)
{
	if (eventType == EventType::Quit)
		isRunning = false;
}

void Application::Run()
{
	while (isRunning)
	{
		Time::Tick();
		Input::Update();

		// TODO: TESTING CODE
		if (Input::GetKeyDown(SDL_SCANCODE_P))
		{
			if (RuntimeConfig::IsInPlayMode())
				RuntimeConfig::SetMode(RuntimeConfig::Mode::Editor);
			else
				RuntimeConfig::SetMode(RuntimeConfig::Mode::Play);
		}

		editorRuntime.Update(Time::DeltaTime());

		if (RuntimeConfig::HasEnteredPlayMode())
		{
			engineRuntime.Start();
			RuntimeConfig::PlayModeEntered();
		}

		if (RuntimeConfig::IsInPlayMode())
		{
			engineRuntime.Update(Time::DeltaTime());
			engineRuntime.FixedUpate(TimeConfig::FIXED_DELTA_TIME);
		}

		// INFO: Internal Runtime Config Checks to accompany editor VS play mode cameras
		engineRuntime.LateUpdate(Time::DeltaTime());

		renderer.RenderFrame(engineRuntime.GetScene());
		editorRuntime.RenderGUI();

		EventDispatcher::ProcessEvents();
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
