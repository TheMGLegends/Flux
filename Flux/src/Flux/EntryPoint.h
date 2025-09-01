#pragma once

#include "Flux/Application.h"
#include "Flux/Logging/Log.h"

#ifdef FLUX_PLATFORM_WINDOWS

extern std::unique_ptr<Flux::Application> Flux::CreateApplication();

int main(int argc, char** argv)
{
	Flux::Log::Initialise();

	std::unique_ptr<Flux::Application> app = Flux::CreateApplication();

	if (!app) 
	{ 
		FLUX_CORE_ERROR("Application could not be created, did you select the correct startup project? (Debug = FluxEditor, Release = Sandbox)"); 
		return -1; 
	}

	app->Run();

	return 0;
}

#else
	#error Application could not be run! Unknown platform!
#endif