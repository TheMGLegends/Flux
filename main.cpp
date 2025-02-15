#include <iostream>

#include <SDL3/SDL.h>
#include <Mouse.h>
#include <nlohmann/json.hpp>

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

	return 0;
}