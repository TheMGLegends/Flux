#pragma once

#ifdef FLUX_PLATFORM_WINDOWS

extern std::unique_ptr<Flux::Application> Flux::CreateApplication();

int main(int argc, char** argv)
{
	Flux::Log::Initialise();

	std::unique_ptr<Flux::Application> app = Flux::CreateApplication();
	app->Run();
}

#else
	#error Application could not be run! Unknown platform!
#endif