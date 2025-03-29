#include "Application.h"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

#include "Core/GlobalDefines.h"
#include "Core/Configs/EditorConfig.h"
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
#include "Engine/Scene/Scene.h"
#include "Engine/Entities/Components/PhysicsBody.h"
#include "Engine/Physics/Physics.h"

using namespace Flux;
using namespace Flux::GlobalDefines;

Application::Application() : window(nullptr), isRunning(false)
{
	if (!SDL_Init(SDL_INIT_VIDEO /*| SDL_INIT_GAMEPAD*/))
	{
		Debug::LogError("Application::Application() - Failed to initialise SDL Systems");
	}

	// INFO: Window Creation
	window = SDL_CreateWindow("Flux Engine", EngineConfig::windowWidth, EngineConfig::windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

	if (!window)
	{
		Debug::LogError("Application::Application() - Failed to create SDL Window");
	}

	if (IS_FAILURE(Input::Initialise(window)))
	{
		Debug::LogError("Application::Application() - Failed to initialise Input System");
	}

	// INFO: Update Window Size Config based on SDL Window Size
	SDL_GetWindowSize(window, &EngineConfig::windowWidth, &EngineConfig::windowHeight);

	if (FAILED(renderer.Initialise(GetWindowHandle())))
	{
		Debug::LogError("Application::Application() - Failed to initialise Renderer");
	}

	if (FAILED(AssetHandler::Initialise(renderer.GetDevice(), renderer.GetDeviceContext())))
	{
		Debug::LogError("Application::Application() - Failed to initialise Asset Handler");
	}

	if (IS_FAILURE(editorRuntime.PreInitialise(window, renderer.GetDevice(), renderer.GetDeviceContext())))
	{
		Debug::LogError("Application::Application() - Failed to pre-initialise Editor Runtime");
	}
	
	if (IS_FAILURE(engineRuntime.PreInitialise()))
	{
		Debug::LogError("Application::Application() - Failed to pre-initialise Engine Runtime");
	}

	if (IS_FAILURE(editorRuntime.Initialise(renderer)))
	{
		Debug::LogError("Application::Application() - Failed to initialise Editor Runtime");
	}

	if (IS_FAILURE(engineRuntime.Initialise()))
	{
		Debug::LogError("Application::Application() - Failed to initialise Engine Runtime");
	}

	// INFO: Setup Events to Listen For
	EventDispatcher::AddListener(EventType::Quit, this);

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

		editorRuntime.Update(Time::DeltaTime());

		if (RuntimeConfig::HasEnteredPlayMode() && !RuntimeConfig::IsPaused())
		{
			// TODO: TESTING CODE
			Audio::PlaySound3D("testSound", FMOD_VECTOR{ 0.0f, 0.0f, 10.0f }, 1.0f, 500.0f, true);

			engineRuntime.Start();
			RuntimeConfig::PlayModeEntered();
		}

		if (RuntimeConfig::IsInPlayMode() && !RuntimeConfig::IsPaused())
		{
			// TODO: TESTING CODE
			if (Input::GetKeyDown(SDL_SCANCODE_O))
			{
				if (RuntimeConfig::IsInPlayMode())
				{
					auto physicsBodies = engineRuntime.GetScene().GetComponents<PhysicsBody>();

					for (auto& physicBody : physicsBodies)
					{
						if (physicBody.expired())
							continue;

						std::shared_ptr<PhysicsBody> physicsBody = physicBody.lock();
						physicsBody->AddForce(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 100.0f), physx::PxForceMode::eIMPULSE);
					}
				}
			}

			engineRuntime.Update(Time::DeltaTime());
			engineRuntime.FixedUpate(TimeConfig::FIXED_DELTA_TIME);
		}

		// INFO: Internal Runtime Config Checks to accompany editor VS play mode cameras
		engineRuntime.LateUpdate(Time::DeltaTime());

		// INFO: Renders the scene and ImGui
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
