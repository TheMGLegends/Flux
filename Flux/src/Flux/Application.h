#pragma once

#include "Core.h"

namespace Flux
{
	class FLUX_API Application
	{
	public:
		Application() = default;
		virtual ~Application() = default;

		void Run();
	};

	// INFO: Defined by client to create custom entry point
	Application* CreateApplication();
}