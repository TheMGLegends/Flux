#include "FluxEditorApplication.h"

#ifdef FLUX_DEBUG
std::unique_ptr<Flux::Application> Flux::CreateApplication()
{
	return std::make_unique<FluxEditorApplication>();
}
#endif