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

	Application::Application(bool isStandalone) : window(nullptr), isRunning(false)
	{
		// INFO: Set Random Seed
		srand(static_cast<unsigned int>(time(nullptr)));

		if (isStandalone) { RuntimeConfig::EnableStandalone(); }

		// INFO: Disable Support for DirectInput (Legacy API), now loads faster
		if (EngineConfig::DIRECT_INPUT_DISABLED)
		{
			SDL_SetHint(SDL_HINT_JOYSTICK_DIRECTINPUT, "0");
		}
		else
		{
			Debug::LogWarning("Application::Application() - DirectInput support is enabled, as a result your application may take longer to start up");
		}

		// INFO: SDL Initialisation
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
		{
			Debug::LogError("Application::Application() - Failed to initialise SDL Systems");
			return;
		}

		window = SDL_CreateWindow("Flux Engine", EngineConfig::GetWindowWidth(), EngineConfig::GetWindowHeight(), SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

		if (!window)
		{
			Debug::LogError("Application::Application() - Failed to create SDL Window");
			return;
		}

		SDL_GetWindowSize(window, &EngineConfig::WindowWidthRef(), &EngineConfig::WindowHeightRef());

		// INFO: Input System Initialisation
		if (FLUX_FAIL(Input::Initialise(window)))
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

		if (FLUX_FAIL(renderer.PostInitialise()))
		{
			Debug::LogError("Application::Application() - Failed to post-initialise Renderer");
			return;
		}

		// INFO: Engine Runtime Initialisation
		if (FLUX_FAIL(engineRuntime.PreInitialise()))
		{
			Debug::LogError("Application::Application() - Failed to pre-initialise Engine Runtime");
			return;
		}

		if (FLUX_FAIL(engineRuntime.Initialise()))
		{
			Debug::LogError("Application::Application() - Failed to initialise Engine Runtime");
			return;
		}

		if (!isStandalone)
		{
			// INFO: Editor Runtime Initialisation
			if (FLUX_FAIL(editorRuntime.PreInitialise(window, renderer.GetDevice(), renderer.GetDeviceContext())))
			{
				Debug::LogError("Application::Application() - Failed to pre-initialise Editor Runtime");
				return;
			}

			if (FLUX_FAIL(editorRuntime.Initialise(renderer)))
			{
				Debug::LogError("Application::Application() - Failed to initialise Editor Runtime");
				return;
			}
		}

		// INFO: Event Listener Registration
		if (FLUX_FAIL(EventDispatcher::AddListener(EventType::Quit, this)))
		{
			Debug::LogError("Application::Application() - Failed to add Quit event listener");
			return;
		}

		isRunning = true;
	}

	Application::~Application()
	{
		engineRuntime.Release();
		if (!RuntimeConfig::IsStandalone()) { editorRuntime.Release(); }
		Input::Release();

		EventDispatcher::RemoveListener(this);

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

			if (!RuntimeConfig::IsStandalone()) { editorRuntime.Update(deltaTime); }

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

		return static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
	}
}
