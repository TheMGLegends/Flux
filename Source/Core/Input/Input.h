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

		/// @return 0 if successful
		static int Initialise(SDL_Window* _window);

		static void Update();

		static void Release();

		static inline bool GetKey(SDL_Scancode key) { return currentKeyboardState[key]; }
		static inline bool GetKeyDown(SDL_Scancode key) { return currentKeyboardState[key] && !previousKeyboardState[key]; }
		static inline bool GetKeyUp(SDL_Scancode key) { return !currentKeyboardState[key] && previousKeyboardState[key]; }

		static inline bool GetMouseButton(SDL_MouseButtonFlags button) { return currentMouseState & SDL_BUTTON_MASK(button); }
		static inline bool GetMouseButtonDown(SDL_MouseButtonFlags button) { return (currentMouseState & SDL_BUTTON_MASK(button)) && !(previousMouseState & SDL_BUTTON_MASK(button)); }
		static inline bool GetMouseButtonUp(SDL_MouseButtonFlags button) { return !(currentMouseState & SDL_BUTTON_MASK(button)) && (previousMouseState & SDL_BUTTON_MASK(button)); }

		static inline const DirectX::SimpleMath::Vector2& GetMousePosition() { return mousePosition; }
		/// @param isRelative: true = relative, false = absolute
		static void SetMouseMode(bool _isRelative);

		/// @brief Get the vertical scroll of the mouse wheel
		/// @param verticalScroll: The variable that will be populated with the vertical scroll value
		/// @return true if the scroll wheel is being scrolled (not 0)
		static bool GetMouseVerticalScroll(float& verticalScroll);

		static inline bool GetGamepadButton(SDL_GamepadButton button) { return currentGamepadButtonState[button]; }
		static inline bool GetGamepadButtonDown(SDL_GamepadButton button) { return currentGamepadButtonState[button] && !previousGamepadButtonState[button]; }
		static inline bool GetGamepadButtonUp(SDL_GamepadButton button) { return !currentGamepadButtonState[button] && previousGamepadButtonState[button]; }

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
		static inline const int DEADZONE = 8000;
	};
}

