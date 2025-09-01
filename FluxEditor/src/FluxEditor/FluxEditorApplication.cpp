#include "FluxEditorApplication.h"

#if defined(FLUX_DEBUG)
std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
	return std::make_unique<FluxEditorApplication>();
}
#elif defined(FLUX_RELEASE)
std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
	return nullptr;
}
#endif