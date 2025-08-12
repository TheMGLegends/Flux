#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

extern Flux::Application* Flux::CreateApplication();

int main(int argc, char** argv)
{
	Flux::Log::Initialise();

	Flux::Application* app = Flux::CreateApplication();
	app->Run();
	delete app;
	app = nullptr;
}

#else
	#error Application could not be run! Unknown platform!
#endif