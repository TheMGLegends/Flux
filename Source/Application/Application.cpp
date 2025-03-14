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

	isRunning = true;
}

Application::~Application()
{
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

		editorRuntime.Update(Time::DeltaTime());

		if (RuntimeConfig::HasEnteredPlayMode())
		{
			engineRuntime.Start();
			RuntimeConfig::PlayModeEntered();
		}

		if (RuntimeConfig::IsInPlayMode())
			engineRuntime.FixedUpate(TimeConfig::FIXED_DELTA_TIME);
		
		// INFO: Internal Runtime Config Checks to accompany editor VS play mode cameras
		engineRuntime.Update(Time::DeltaTime());

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
