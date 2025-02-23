#pragma once

// TODO: Include renderer

#include "Runtimes/EditorRuntime.h"
#include "Runtimes/EngineRuntime.h"

struct SDL_Window;

class Application
{
public:
	Application();
	~Application();

	void Run();

private:
	//EditorRuntime editorRuntime;
	//EngineRuntime engineRuntime;
	// TODO: EventDispatcher
	//EventDispatcher eventDispatcher;

	// TODO: Renderer
	SDL_Window* window;

	bool isRunning;
};

