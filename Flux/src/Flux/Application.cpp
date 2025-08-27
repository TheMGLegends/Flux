#include "FluxPCH.h"

#include "Application.h"

#include "Flux/Events/ApplicationEvent.h"

namespace Flux
{
	Application::Application() : isRunning(false)
	{
		window = std::make_unique<Window>();
		FLUX_CORE_ASSERT(window != nullptr, "Failed to create Flux Window!");
		window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));

		isRunning = true;
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::OnWindowClose));

		// TODO: Could also just create lambdas here instead of binding functions
		//dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent&) { isRunning = false; return true; });

		// INFO: Propagate Event to Layers
		layerManager.OnEvent(event);
	}

	void Application::Run()
	{
		while (isRunning)
		{
			layerManager.Update();
			window->Update();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		isRunning = false;
		return true;
	}
}