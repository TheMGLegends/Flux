#include "Input.h"

using namespace DirectX::SimpleMath;

const bool* Input::currentKeyboardState = nullptr;
bool* Input::previousKeyboardState = nullptr;
int Input::keyLength = 0;

SDL_MouseButtonFlags Input::currentMouseState = 0;
SDL_MouseButtonFlags Input::previousMouseState = 0;

Vector2 Input::mousePosition = Vector2::Zero;

bool Input::PreInitialise()
{
	SDL_InitFlags flags = SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC;

	if (!SDL_InitSubSystem(flags))
	{
		// TODO: Logging System Log Error Message
		return false;
	}

	return true; // INFO: Pre-Initialisation Successful
}

bool Input::Initialise()
{
	currentKeyboardState = SDL_GetKeyboardState(&keyLength);

	if (!currentKeyboardState)
	{
		// TODO: Logging System Log Error Message
		return false;
	}

	previousKeyboardState = new bool[keyLength];
	memcpy(previousKeyboardState, currentKeyboardState, keyLength);

	if (!previousKeyboardState)
	{
		// TODO: Logging System Log Error Message
		return false;
	}

	currentMouseState = SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
	previousMouseState = currentMouseState;

	return true; // INFO: Initialisation Successful
}

void Input::Update()
{
	memcpy(previousKeyboardState, currentKeyboardState, keyLength);
	previousMouseState = currentMouseState;
	currentMouseState = SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
		{
			// TODO: Event System Dispatch Quit Event
			break;
		}
		default:
			break;
		}
	}
}

void Input::Release()
{
	if (previousKeyboardState)
	{
		delete[] previousKeyboardState;
		previousKeyboardState = nullptr;
	}
}
