#include <iostream>
#include <Windows.h>

#include <SDL3/SDL.h>

#include <Mouse.h>

#include <nlohmann/json.hpp>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fmod.hpp>

int main()
{
	// INFO: SDL Integration Test
	/*SDL_InitFlags flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC |
		SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_CAMERA;
	if (SDL_Init(flags))
	{
		std::cout << "Initialized SDL successfully!" << std::endl;
	}

	SDL_Quit();*/

	// INFO: DirectXTK Integration Test
	//DirectX::Mouse mouse;

	// INFO: JSON Integration Test
	/*nlohmann::ordered_json j = {
		{"pi", 3.141},
		{"happy", true},
		{"name", "Niels"},
		{"nothing", nullptr},
		{"answer", {
			{"everything", 42}
		}},
		{"list", {1, 0, 2}},
		{"object", {
			{"currency", "USD"},
			{"value", 42.99}
		}}
	};

	std::cout << j.dump(4) << std::endl;*/

	// INFO: Assimp Integration Test
	/*const aiScene* testImport = aiImportFile("Assets/Models/Cube.obj", aiProcessPreset_TargetRealtime_MaxQuality);

	if (testImport)
	{
		std::cout << "Imported model successfully!" << std::endl;
	}*/

	// INFO: FreeType Integration Test
	/*FT_Library library;
	if (FT_Init_FreeType(&library) == FT_Err_Ok)
	{
		std::cout << "Initialized FreeType successfully!" << std::endl;
	}*/

	// INFO: FMOD Integration Test
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	FMOD_RESULT result;
	FMOD::System* system;

	result = FMOD::System_Create(&system);
	if (result == FMOD_OK)
	{
		std::cout << "Created FMOD system successfully!" << std::endl;
	}

	system->release();
	CoUninitialize();

	return 0;
}