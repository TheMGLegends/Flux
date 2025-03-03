#pragma once

#include "Core/EventSystem/IEventListener.h"

#include "Runtimes/EditorRuntime.h"
#include "Runtimes/EngineRuntime.h"
#include "Core/EventSystem/EventDispatcher.h"
#include "Core/Renderer/Renderer.h"

struct SDL_Window;

namespace Flux
{
	class Application : public IEventListener
	{
	public:
		Application();
		~Application();

		virtual void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		void Run();

	private:
		HWND GetWindowHandle() const;

	private:
		EventDispatcher eventDispatcher;

		EditorRuntime editorRuntime;
		EngineRuntime engineRuntime;

		SDL_Window* window;
		Renderer renderer;

		bool isRunning;
	};
}

