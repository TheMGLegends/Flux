#include "FluxPCH.h" // INFO: Precompiled Header

#include "Application.h"

#include "Logging/Log.h"

namespace Flux
{
	void Application::Run()
	{
		FLUX_CORE_INFO("Hello");

		while (true) {}
	}
}