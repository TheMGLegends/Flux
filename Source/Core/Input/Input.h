#pragma once

#include <SDL3/SDL.h>
#include <SimpleMath.h>

namespace Flux
{
	enum class GamepadJoystick
	{
		Left,
		Right
	};

	class Input
	{
	public:
		Input() = delete;
		~Input() = delete;
		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;

		static int Initialise(SDL_Window* _window);
		static void Update();
		static void Release();

		static bool GetKey(SDL_Scancode key);
		static bool GetKeyDown(SDL_Scancode key);
		static bool GetKeyUp(SDL_Scancode key);

		static bool GetMouseButton(SDL_MouseButtonFlags button);
		static bool GetMouseButtonDown(SDL_MouseButtonFlags button);
		static bool GetMouseButtonUp(SDL_MouseButtonFlags button);

		static const DirectX::SimpleMath::Vector2& GetMousePosition();
		static void SetMousePosition(const DirectX::SimpleMath::Vector2& position);

		/// @param isRelative: true = relative, false = absolute
		static void SetMouseMode(bool _isRelative);
		static bool IsMouseRelative();

		/// @brief Get the vertical scroll of the mouse wheel
		/// @param verticalScroll: The variable that will be populated with the vertical scroll value
		/// @return true if the scroll wheel is being scrolled (not 0)
		static bool GetMouseVerticalScroll(float& verticalScroll);

		static bool GetGamepadButton(SDL_GamepadButton button);
		static bool GetGamepadButtonDown(SDL_GamepadButton button);
		static bool GetGamepadButtonUp(SDL_GamepadButton button);

		/// @param trigger: Only works with SDL_GAMEPAD_AXIS_LEFT_TRIGGER and SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
		/// @param axisState: Optional parameter to get the axis state of the trigger between 0 and 1
		static bool GetTrigger(SDL_GamepadAxis trigger, float* axisState = nullptr);

		/// @param trigger: Only works with SDL_GAMEPAD_AXIS_LEFT_TRIGGER and SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
		/// @param axisState: Optional parameter to get the axis state of the trigger between 0 and 1
		static bool GetTriggerDown(SDL_GamepadAxis trigger, float* axisState = nullptr);

		/// @param trigger: Only works with SDL_GAMEPAD_AXIS_LEFT_TRIGGER and SDL_GAMEPAD_AXIS_RIGHT_TRIGGER
		/// @param axisState: Optional parameter to get the axis state of the trigger between 0 and 1
		static bool GetTriggerUp(SDL_GamepadAxis trigger, float* axisState = nullptr);

		static DirectX::SimpleMath::Vector2 GetJoystickAxes(GamepadJoystick joystick, bool inverseY = true);

	private:
		static int InitialiseGamepad();
		static void PollEvents();

		/// @brief Populates the currentGamepadState array with the current state of all buttons
		static void GetGamepadButtonState();

		/// @brief Populates the currentGamepadAxisState array with the current state of all axes
		static void GetGamepadAxisState();

	private:
		static SDL_Window* window;

		static const bool* currentKeyboardState;
		static bool* previousKeyboardState;
		static int keyLength;

		static SDL_MouseButtonFlags currentMouseState;
		static SDL_MouseButtonFlags previousMouseState;
		static DirectX::SimpleMath::Vector2 mousePosition;
		static DirectX::SimpleMath::Vector2 latestAbsoluteMousePosition;
		static bool isRelative;
		static float mouseVerticalScroll;

		static SDL_Gamepad* gamepad;
		static bool* currentGamepadButtonState;
		static bool* previousGamepadButtonState;
		static int gamepadButtonLength;

		static Sint16* currentGamepadAxisState;
		static Sint16* previousGamepadAxisState;
		static int gamepadAxisLength;
		static const int DEADZONE;
	};
}

