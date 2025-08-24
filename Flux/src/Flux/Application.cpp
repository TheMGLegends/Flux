#include "FluxPCH.h"

#include "Application.h"

namespace Flux
{
	Application::Application() : isRunning(false)
	{
		window = std::make_unique<Window>();
		FLUX_CORE_ASSERT(window != nullptr, "Failed to create Flux Window!");

		isRunning = true;
	}

	void Application::Run()
	{
		while (isRunning)
		{
			window->Update();
		}
	}
}