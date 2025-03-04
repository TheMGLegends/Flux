#include "Application.h"

#include <SDL3/SDL.h>

#include "Core/Configs/EngineConfig.h"
#include "Core/Configs/RuntimeConfig.h"
#include "Core/Configs/TimeConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Input/Input.h"
#include "Core/Time/Time.h"

// TODO: TESTING INCLUDES
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include <assimp/postprocess.h>
#include "Core/Renderer/Model.h"
#include "Engine/Scene/SceneContext.h"

using namespace Flux;

Application::Application() : window(nullptr), isRunning(false)
{
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		Debug::LogError("Application::Application() - Failed to initialise SDL Video Subsystem");
	}

	// INFO: Window Creation
	window = SDL_CreateWindow("Flux Engine", EngineConfig::windowWidth, EngineConfig::windowHeight, SDL_WINDOW_RESIZABLE);

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
	if (FAILED(renderer.Initialise(GetWindowHandle(), /*TODO: Temporary*/Viewport(0.0f, 0.0f, (float)EngineConfig::windowWidth, (float)EngineConfig::windowHeight, 0.0f, 1.0f))))
	{
		Debug::LogError("Application::Application() - Failed to initialise Renderer");
	}

	editorRuntime.PreInitialise();
	engineRuntime.PreInitialise();

	editorRuntime.Initialise();
	engineRuntime.Initialise();

	SceneContext::GetScene();

	// INFO: Setup Events to Listen For
	EventDispatcher::AddListener(EventType::Quit, this);

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
		Input::Update();

		editorRuntime.Update(Time::DeltaTime());

		if (RuntimeConfig::HasEnteredPlayMode())
		{
			engineRuntime.Start();
			RuntimeConfig::PlayModeEntered();
		}

		if (RuntimeConfig::IsInPlayMode())
		{
			engineRuntime.FixedUpate(TimeConfig::FIXED_DELTA_TIME);
			engineRuntime.Update(Time::DeltaTime());
		}

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
