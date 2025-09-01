#include "FluxEditorApplication.h"

#if defined(FLUX_DEBUG)
// INFO: Build the Flux Editor with the Sandbox
std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
	return std::make_unique<FluxEditorApplication>();
}
#elif defined(FLUX_RELEASE) || defined(FLUX_DEVELOPMENT)
// INFO: Build the Sandbox as a Standalone Application
std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
	return std::make_unique<SandboxApplication>();
}
#endif