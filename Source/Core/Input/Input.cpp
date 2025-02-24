#include "Input.h"

using namespace DirectX::SimpleMath;

SDL_Window* Input::window = nullptr;

const bool* Input::currentKeyboardState = nullptr;
bool* Input::previousKeyboardState = nullptr;
int Input::keyLength = 0;

SDL_MouseButtonFlags Input::currentMouseState = 0;
SDL_MouseButtonFlags Input::previousMouseState = 0;
Vector2 Input::mousePosition = Vector2::Zero;
Vector2 Input::latestAbsoluteMousePosition = Vector2::Zero;
bool Input::isRelative = false;

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

bool Input::Initialise(SDL_Window* _window)
{
	window = _window;

	if (!window)
	{
		// TODO: Logging System Log Error Message
	}

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
	if (isRelative)
		currentMouseState = SDL_GetRelativeMouseState(&mousePosition.x, &mousePosition.y);
	else
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

void Input::SetMouseMode(bool _isRelative)
{
	if (!window)
	{
		// TODO: Logging System Log Error Message
		return;
	}

	// INFO: Going from relative to absolute
	if (isRelative && !_isRelative)
	{
		SDL_WarpMouseInWindow(window, latestAbsoluteMousePosition.x, latestAbsoluteMousePosition.y);
		SDL_SetWindowRelativeMouseMode(window, false);
	}
	// INFO: Going from absolute to relative
	else if (!isRelative && _isRelative)
	{
		// INFO: Store the latest absolute mouse position before switching to relative
		latestAbsoluteMousePosition = mousePosition;

		SDL_SetWindowRelativeMouseMode(window, true);
	}

	isRelative = _isRelative;
}
