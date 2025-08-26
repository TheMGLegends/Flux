#pragma once

#include "Core.h"

#include <memory>

#include "Window/Window.h"

namespace Flux
{
	class WindowCloseEvent;

	class FLUX_API Application : public IEventListener
	{
	public:
		Application();
		virtual ~Application() = default;

		virtual void OnEvent(Event& event) override;

		void Run();

	private:
		bool OnWindowClose(WindowCloseEvent& event);

	private:
		bool isRunning;

		std::unique_ptr<Window> window;
	};

	// INFO: Defined by client to create custom entry point 
	std::unique_ptr<Application> CreateApplication();
}