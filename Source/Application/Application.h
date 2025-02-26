#pragma once

#include "Runtimes/EditorRuntime.h"
#include "Runtimes/EngineRuntime.h"
#include "../Core/Renderer/Renderer.h"

struct SDL_Window;

class Application
{
public:
	Application();
	~Application();

	void Run();

private:
	HWND GetWindowHandle() const;

private:
	//EditorRuntime editorRuntime;
	//EngineRuntime engineRuntime;
	// TODO: EventDispatcher
	//EventDispatcher eventDispatcher;

	SDL_Window* window;
	Renderer renderer;

	bool isRunning;
};

