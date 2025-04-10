#include "Input.h"

#include <cmath>
#include <backends/imgui_impl_sdl3.h>

#include "Core/GlobalDefines.h"

#include "Core/Configs/EngineConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/EventSystem/EventDispatcher.h"

using namespace DirectX::SimpleMath;

namespace Flux
{
	using namespace GlobalDefines;

	SDL_Window* Input::window = nullptr;

	const bool* Input::currentKeyboardState = nullptr;
	bool* Input::previousKeyboardState = nullptr;
	int Input::keyLength = 0;

	SDL_MouseButtonFlags Input::currentMouseState = 0;
	SDL_MouseButtonFlags Input::previousMouseState = 0;
	Vector2 Input::mousePosition = Vector2::Zero;
	Vector2 Input::latestAbsoluteMousePosition = Vector2::Zero;
	bool Input::isRelative = false;
	float Input::mouseVerticalScroll = 0.0f;

	SDL_Gamepad* Input::gamepad = nullptr;
	bool* Input::currentGamepadButtonState = nullptr;
	bool* Input::previousGamepadButtonState = nullptr;
	int Input::gamepadButtonLength = 0;
	Sint16* Input::currentGamepadAxisState = nullptr;
	Sint16* Input::previousGamepadAxisState = nullptr;
	int Input::gamepadAxisLength = 0;
	const int Input::DEADZONE = 8000;

	int Input::Initialise(SDL_Window* _window)
	{
		window = _window;

		if (!window)
		{
			Debug::LogError("Input::Initialise() - Window is nullptr");
			return FLUX_FAILURE;
		}

		// INFO: Keyboard Input Initialisation
		currentKeyboardState = SDL_GetKeyboardState(&keyLength);

		if (!currentKeyboardState)
		{
			Debug::LogError("Input::Initialise() - Failed to get Keyboard State");
			return FLUX_FAILURE;
		}

		previousKeyboardState = new bool[keyLength];
		memcpy(previousKeyboardState, currentKeyboardState, keyLength);

		if (!previousKeyboardState)
		{
			Debug::LogError("Input::Initialise() - Failed to allocate memory for previous Keyboard State");
			return FLUX_FAILURE;
		}

		// INFO: Mouse Input Initialisation
		currentMouseState = SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
		previousMouseState = currentMouseState;

		// INFO: Gamepad Input Initialisation
		int count = 0;
		SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);

		if (count > 0 && joysticks)
		{
			for (size_t i = 0; i < count; i++)
			{
				if (SDL_IsGamepad(joysticks[i]))
				{
					gamepad = SDL_OpenGamepad(joysticks[i]);
					Debug::Log("Input::Initialise() - Gamepad Detected: " + std::string(SDL_GetGamepadName(gamepad)));
					break;
				}
			}
		}

		if (gamepad)
		{
			currentGamepadButtonState = new bool[SDL_GAMEPAD_BUTTON_COUNT];
			gamepadButtonLength = SDL_GAMEPAD_BUTTON_COUNT * sizeof(bool);
			memset(currentGamepadButtonState, 0, gamepadButtonLength);
			GetGamepadButtonState();

			if (!currentGamepadButtonState)
			{
				Debug::LogError("Input::Initialise() - Failed to allocate memory for current Gamepad Button State");
				return FLUX_FAILURE;
			}

			previousGamepadButtonState = new bool[SDL_GAMEPAD_BUTTON_COUNT];
			memcpy(previousGamepadButtonState, currentGamepadButtonState, gamepadButtonLength);

			if (!previousGamepadButtonState)
			{
				Debug::LogError("Input::Initialise() - Failed to allocate memory for previous Gamepad Button State");
				return FLUX_FAILURE;
			}

			currentGamepadAxisState = new Sint16[SDL_GAMEPAD_AXIS_COUNT];
			gamepadAxisLength = SDL_GAMEPAD_AXIS_COUNT * sizeof(Sint16);
			memset(currentGamepadAxisState, 0, gamepadAxisLength);
			GetGamepadAxisState();

			if (!currentGamepadAxisState)
			{
				Debug::LogError("Input::Initialise() - Failed to allocate memory for current Gamepad Axis State");
				return FLUX_FAILURE;
			}

			previousGamepadAxisState = new Sint16[SDL_GAMEPAD_AXIS_COUNT];
			memcpy(previousGamepadAxisState, currentGamepadAxisState, gamepadAxisLength);
		}

		return FLUX_SUCCESS;
	}

	void Input::Update()
	{
		memcpy(previousKeyboardState, currentKeyboardState, keyLength);

		previousMouseState = currentMouseState;
		if (isRelative)
		{
			Vector2 newMousePosition;
			SDL_MouseButtonFlags newMouseState = SDL_GetRelativeMouseState(&newMousePosition.x, &newMousePosition.y);

			// INFO: Keep the mouse at the same position to prevent ImGui interactions granted the mouse has moved
			if (newMousePosition != mousePosition)
			{
				// INFO: Only warps if the mouse has moved
				SDL_WarpMouseInWindow(window, latestAbsoluteMousePosition.x, latestAbsoluteMousePosition.y);
				mousePosition = newMousePosition;
			}

			currentMouseState = newMouseState;
		}
		else
		{
			currentMouseState = SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
		}

		// INFO: Reset the vertical scroll value
		mouseVerticalScroll = 0.0f;

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
			// INFO: ImGui Event Handling
			ImGui_ImplSDL3_ProcessEvent(&event);

			switch (event.type)
			{
			case SDL_EVENT_QUIT:
			{
				EventDispatcher::QueueEvent(EventType::Quit, nullptr);
				break;
			}
			case SDL_EVENT_WINDOW_RESIZED:
			{
				// INFO: Update global window size variables
				EngineConfig::windowWidth = event.window.data1;
				EngineConfig::windowHeight = event.window.data2;

				EventDispatcher::QueueEvent(EventType::WindowResized, nullptr);
				break;
			}
			case SDL_EVENT_MOUSE_WHEEL:
			{
				mouseVerticalScroll = event.wheel.y;
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
	}

	void Input::SetMouseMode(bool _isRelative)
	{
		if (!window)
		{
			Debug::LogError("Input::SetMouseMode() - Window is nullptr");
			return;
		}

		// INFO: Going from relative to absolute
		if (isRelative && !_isRelative)
		{
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

	bool Input::GetMouseVerticalScroll(float& verticalScroll)
	{
		if (mouseVerticalScroll == 0.0f) { return false; }

		verticalScroll = mouseVerticalScroll;
		return true;
	}

	bool Input::GetTrigger(SDL_GamepadAxis trigger, float* axisState)
	{
		if (trigger != SDL_GAMEPAD_AXIS_LEFT_TRIGGER && trigger != SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) { return false; }

		// INFO: Normalize the axis (0 to 1)
		if (axisState) { *axisState = currentGamepadAxisState[trigger] / (float)SDL_JOYSTICK_AXIS_MAX; }

		return currentGamepadAxisState[trigger] > 0;
	}

	bool Input::GetTriggerDown(SDL_GamepadAxis trigger, float* axisState)
	{
		if (trigger != SDL_GAMEPAD_AXIS_LEFT_TRIGGER && trigger != SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) { return false; }

		// INFO: Normalize the axis (0 to 1)
		if (axisState) { *axisState = currentGamepadAxisState[trigger] / (float)SDL_JOYSTICK_AXIS_MAX; }

		return currentGamepadAxisState[trigger] > 0 && previousGamepadAxisState[trigger] == 0;
	}

	bool Input::GetTriggerUp(SDL_GamepadAxis trigger, float* axisState)
	{
		if (trigger != SDL_GAMEPAD_AXIS_LEFT_TRIGGER && trigger != SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) { return false; }

		// INFO: Normalize the axis (0 to 1)
		if (axisState) { *axisState = currentGamepadAxisState[trigger] / (float)SDL_JOYSTICK_AXIS_MAX; }

		return currentGamepadAxisState[trigger] == 0 && previousGamepadAxisState[trigger] > 0;
	}

	Vector2 Input::GetJoystickAxes(GamepadJoystick joystick, bool inverseY)
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
		axes.y = axes.y < 0 ? axes.y / (float)(SDL_JOYSTICK_AXIS_MAX + 1) : axes.y / (float)SDL_JOYSTICK_AXIS_MAX;

		if (inverseY && axes.y != 0) { axes.y *= -1; }

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
}
