#pragma once

#include "Core.h"

#include <memory>

#include "Window/Window.h"

namespace Flux
{
	class FLUX_API Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();

	private:
		bool isRunning;

		std::unique_ptr<Window> window;
	};

	// INFO: Defined by client to create custom entry point 
	Application* CreateApplication();
}