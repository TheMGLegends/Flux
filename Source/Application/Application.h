#pragma once

#include "Core/EventSystem/IEventListener.h"

#include "Core/Renderer/Renderer.h"
#include "Runtimes/EditorRuntime.h"
#include "Runtimes/EngineRuntime.h"

struct SDL_Window;

namespace Flux
{
	class Application : public IEventListener
	{
	public:
		/// @param isStandalone If true, the application will run in standalone mode (no editor runtime, just the game)
		explicit Application(bool isStandalone = false);
		~Application() override;

		void OnNotify(EventType eventType, std::shared_ptr<Event> event) override;

		void Run();

	private:
		HWND GetWindowHandle() const;

	private:
		SDL_Window* window;
		Renderer renderer;

		EditorRuntime editorRuntime;
		EngineRuntime engineRuntime;

		bool isRunning;
	};
}

