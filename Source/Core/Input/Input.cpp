#include "Input.h"

#include <cmath> // TODO: Replace with custom library maybe
#include <iostream> // TODO: TESTING

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

SDL_Gamepad* Input::gamepad = nullptr;
bool* Input::currentGamepadButtonState = nullptr;
bool* Input::previousGamepadButtonState = nullptr;
int Input::gamepadButtonLength = 0;
Sint16* Input::currentGamepadAxisState = nullptr;
Sint16* Input::previousGamepadAxisState = nullptr;
int Input::gamepadAxisLength = 0;

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


	int count = 0;
	SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);

	if (count > 0 && joysticks)
	{
		for (size_t i = 0; i < count; i++)
		{
			if (SDL_IsGamepad(joysticks[i]))
			{
				gamepad = SDL_OpenGamepad(joysticks[i]);
				// TODO: Logging System Log Message
				break;
			}
		}
	}

	if (gamepad)
	{
		currentGamepadButtonState = new bool[SDL_GAMEPAD_BUTTON_COUNT];
		gamepadButtonLength = SDL_GAMEPAD_BUTTON_COUNT * sizeof(bool);
		GetGamepadButtonState();

		if (!currentGamepadButtonState)
		{
			// TODO: Logging System Log Error Message
			return false;
		}

		previousGamepadButtonState = new bool[SDL_GAMEPAD_BUTTON_COUNT];
		memcpy(previousGamepadButtonState, currentGamepadButtonState, gamepadButtonLength);

		if (!previousGamepadButtonState)
		{
			// TODO: Logging System Log Error Message
			return false;
		}

		currentGamepadAxisState = new Sint16[SDL_GAMEPAD_AXIS_COUNT];
		gamepadAxisLength = SDL_GAMEPAD_AXIS_COUNT * sizeof(Sint16);
		GetGamepadAxisState();

		if (!currentGamepadAxisState)
		{
			// TODO: Logging System Log Error Message
			return false;
		}

		previousGamepadAxisState = new Sint16[SDL_GAMEPAD_AXIS_COUNT];
		memcpy(previousGamepadAxisState, currentGamepadAxisState, gamepadAxisLength);
	}

	//SDL_GetGamepadAxis

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

	if (gamepad)
	{
		memcpy(previousGamepadButtonState, currentGamepadButtonState, gamepadButtonLength);
		GetGamepadButtonState();

		memcpy(previousGamepadAxisState, currentGamepadAxisState, gamepadAxisLength);
		GetGamepadAxisState();
	}

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
		case SDL_EVENT_GAMEPAD_AXIS_MOTION:
		{
			/*if (event.gaxis.axis == 0)
			{
				std::cout << "Left Stick X-Axis: " << event.gaxis.value << std::endl;
			}
			else if (event.gaxis.axis == 1)
			{
				std::cout << "Left Stick Y-Axis: " << event.gaxis.value << std::endl;
			}
			else if (event.gaxis.axis == 2)
			{
				std::cout << "Right Stick X-Axis: " << event.gaxis.value << std::endl;
			}
			else if (event.gaxis.axis == 3)
			{
				std::cout << "Right Stick Y-Axis: " << event.gaxis.value << std::endl;
			}
			else if (event.gaxis.axis == 4)
			{
				std::cout << "Left Trigger: " << event.gaxis.value << std::endl;
			}
			else if (event.gaxis.axis == 5)
			{
				std::cout << "Right Trigger: " << event.gaxis.value << std::endl;
			}*/
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

	if (gamepad)
	{
		SDL_CloseGamepad(gamepad);
		gamepad = nullptr;

		if (currentGamepadButtonState)
		{
			delete[] currentGamepadButtonState;
			currentGamepadButtonState = nullptr;
		}

		if (previousGamepadButtonState)
		{
			delete[] previousGamepadButtonState;
			previousGamepadButtonState = nullptr;
		}

		if (currentGamepadAxisState)
		{
			delete[] currentGamepadAxisState;
			currentGamepadAxisState = nullptr;
		}

		if (previousGamepadAxisState)
		{
			delete[] previousGamepadAxisState;
			previousGamepadAxisState = nullptr;
		}
	}

	SDL_QuitSubSystem(SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC);
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

bool Input::GetTrigger(SDL_GamepadAxis trigger, float* axisState)
{
	if (trigger != SDL_GAMEPAD_AXIS_LEFT_TRIGGER && trigger != SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
		return false;

	// INFO: Normalize the axis (0 to 1)
	if (axisState)
	{
		*axisState = currentGamepadAxisState[trigger] / (float)SDL_JOYSTICK_AXIS_MAX;
	}

	return currentGamepadAxisState[trigger] > 0;
}

bool Input::GetTriggerDown(SDL_GamepadAxis trigger, float* axisState)
{
	if (trigger != SDL_GAMEPAD_AXIS_LEFT_TRIGGER && trigger != SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
		return false;

	// INFO: Normalize the axis (0 to 1)
	if (axisState)
	{
		*axisState = currentGamepadAxisState[trigger] / (float)SDL_JOYSTICK_AXIS_MAX;
	}

	return currentGamepadAxisState[trigger] > 0 && previousGamepadAxisState[trigger] == 0;
}

bool Input::GetTriggerUp(SDL_GamepadAxis trigger, float* axisState)
{
	if (trigger != SDL_GAMEPAD_AXIS_LEFT_TRIGGER && trigger != SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)
		return false;

	// INFO: Normalize the axis (0 to 1)
	if (axisState) 
	{
		*axisState = currentGamepadAxisState[trigger] / (float)SDL_JOYSTICK_AXIS_MAX;
	}

	return currentGamepadAxisState[trigger] == 0 && previousGamepadAxisState[trigger] > 0;
}

Vector2 Input::GetJoystickAxes(GamepadJoystick joystick)
{
	Vector2 axes = Vector2::Zero;

	switch (joystick)
	{
	case GamepadJoystick::Left:
		axes.x = currentGamepadAxisState[SDL_GAMEPAD_AXIS_LEFTX];
		axes.y = currentGamepadAxisState[SDL_GAMEPAD_AXIS_LEFTY];

		break;
	case GamepadJoystick::Right:
		axes.x = currentGamepadAxisState[SDL_GAMEPAD_AXIS_RIGHTX];
		axes.y = currentGamepadAxisState[SDL_GAMEPAD_AXIS_RIGHTY];
		break;
	default:
		break;
	}

	// INFO: Normalize the axes (-1 to 1)
	axes.x = axes.x < 0 ? axes.x / (float)(SDL_JOYSTICK_AXIS_MAX + 1) : axes.x / (float)SDL_JOYSTICK_AXIS_MAX;
	axes.y = axes.y < 0 ? axes.y / (float)SDL_JOYSTICK_AXIS_MIN : axes.y / (float)SDL_JOYSTICK_AXIS_MAX;

	return axes;
}

void Input::GetGamepadButtonState()
{
	if (gamepad)
	{
		for (size_t i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; i++)
		{
			currentGamepadButtonState[i] = SDL_GetGamepadButton(gamepad, (SDL_GamepadButton)i);
		}
	}
}

void Input::GetGamepadAxisState()
{
	if (gamepad)
	{
		for (size_t i = 0; i < SDL_GAMEPAD_AXIS_COUNT; i++)
		{
			Sint16 value = SDL_GetGamepadAxis(gamepad, (SDL_GamepadAxis)i);

			if (std::abs(value) > DEADZONE)
			{
				currentGamepadAxisState[i] = value;
			}
			else
			{
				currentGamepadAxisState[i] = 0;
			}

			currentGamepadAxisState[i];
		}
	}
}
