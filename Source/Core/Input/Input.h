#pragma once

#include <SDL3/SDL.h>
#include <SimpleMath.h>

class Input
{
public:
	Input() = delete;
	~Input() = delete;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	static bool PreInitialise();
	static bool Initialise();

	static void Update();

	static void Release();

	static inline bool GetKey(SDL_Scancode key) { return currentKeyboardState[key]; }
	static inline bool GetKeyDown(SDL_Scancode key) { return currentKeyboardState[key] && !previousKeyboardState[key]; }
	static inline bool GetKeyUp(SDL_Scancode key) { return !currentKeyboardState[key] && previousKeyboardState[key]; }

	static inline bool GetMouseButton(SDL_MouseButtonFlags button) { return currentMouseState & SDL_BUTTON_MASK(button); }
	static inline bool GetMouseButtonDown(SDL_MouseButtonFlags button) { return (currentMouseState & SDL_BUTTON_MASK(button)) && !(previousMouseState & SDL_BUTTON_MASK(button)); }
	static inline bool GetMouseButtonUp(SDL_MouseButtonFlags button) { return !(currentMouseState & SDL_BUTTON_MASK(button)) && (previousMouseState & SDL_BUTTON_MASK(button)); }

	static inline const DirectX::SimpleMath::Vector2& GetMousePosition() { return mousePosition; }

private:
	static const bool* currentKeyboardState;
	static bool* previousKeyboardState;
	static int keyLength;

	static SDL_MouseButtonFlags currentMouseState;
	static SDL_MouseButtonFlags previousMouseState;
	static DirectX::SimpleMath::Vector2 mousePosition;
};

