#include "Application.h"

#include <SDL3/SDL.h>

#include "Core/GlobalDefines.h"

#include "Core/Configs/EngineConfig.h"
#include "Core/Configs/RuntimeConfig.h"
#include "Core/Configs/TimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Input/Input.h"
#include "Core/Renderer/AssetHandler.h"
#include "Core/Time/Time.h"

namespace Flux
{
	using namespace GlobalDefines;

	Application::Application() : window(nullptr), isRunning(false)
	{
		// INFO: SDL Initialisation
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
		{
			Debug::LogError("Application::Application() - Failed to initialise SDL Systems");
			return;
		}

		window = SDL_CreateWindow("Flux Engine", EngineConfig::windowWidth, EngineConfig::windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

		if (!window)
		{
			Debug::LogError("Application::Application() - Failed to create SDL Window");
			return;
		}

		SDL_GetWindowSize(window, &EngineConfig::windowWidth, &EngineConfig::windowHeight);

		// INFO: Input System Initialisation
		if (IS_FAILURE(Input::Initialise(window)))
		{
			Debug::LogError("Application::Application() - Failed to initialise Input System");
			return;
		}

		// INFO: Renderer Initialisation
		if (FAILED(renderer.Initialise(GetWindowHandle())))
		{
			Debug::LogError("Application::Application() - Failed to initialise Renderer");
			return;
		}

		if (FAILED(AssetHandler::Initialise(renderer.GetDevice(), renderer.GetDeviceContext())))
		{
			Debug::LogError("Application::Application() - Failed to initialise Asset Handler");
			return;
		}

		if (IS_FAILURE(renderer.PostInitialise()))
		{
			Debug::LogError("Application::Application() - Failed to post-initialise Renderer");
			return;
		}

		// INFO: Engine Runtime Initialisation
		if (IS_FAILURE(engineRuntime.PreInitialise()))
		{
			Debug::LogError("Application::Application() - Failed to pre-initialise Engine Runtime");
			return;
		}

		if (IS_FAILURE(engineRuntime.Initialise()))
		{
			Debug::LogError("Application::Application() - Failed to initialise Engine Runtime");
			return;
		}

		// INFO: Editor Runtime Initialisation
		if (IS_FAILURE(editorRuntime.PreInitialise(window, renderer.GetDevice(), renderer.GetDeviceContext())))
		{
			Debug::LogError("Application::Application() - Failed to pre-initialise Editor Runtime");
			return;
		}

		if (IS_FAILURE(editorRuntime.Initialise(renderer)))
		{
			Debug::LogError("Application::Application() - Failed to initialise Editor Runtime");
			return;
		}

		// INFO: Event Listener Registration
		if (!EventDispatcher::AddListener(EventType::Quit, this))
		{
			Debug::LogWarning("Application::Application() - Failed to register event listener, listener already exists or was failed to be added");
		}

		isRunning = true;
	}

	Application::~Application()
	{
		engineRuntime.Release();
		editorRuntime.Release();
		Input::Release();

		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void Application::OnNotify(EventType eventType, std::shared_ptr<Event> event)
	{
		if (eventType == EventType::Quit) { isRunning = false; }
	}

	void Application::Run()
	{
		while (isRunning)
		{
			Time::Tick();
			Input::Update();
			float deltaTime = Time::DeltaTime();

			editorRuntime.Update(deltaTime);

			if (RuntimeConfig::HasEnteredPlayMode() && !RuntimeConfig::IsPaused())
			{
				engineRuntime.Start();
				RuntimeConfig::PlayModeEntered();
			}

			if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
			{
				engineRuntime.Update(deltaTime);
				engineRuntime.FixedUpate(TimeConfig::FIXED_DELTA_TIME);
			}

			engineRuntime.LateUpdate(deltaTime);
			renderer.RenderFrame(engineRuntime.GetScene());

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
}
