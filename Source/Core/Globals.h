#pragma once

namespace RuntimeState
{
	enum class Mode
	{
		Editor,
		Play
	};

	namespace Internal
	{
		inline Mode currentMode = Mode::Editor;
		inline bool enteredPlayMode = false;
	}

	inline void SetMode(Mode newMode)
	{
		if (Internal::currentMode == Mode::Editor && newMode == Mode::Play)
			Internal::enteredPlayMode = true;

		Internal::currentMode = newMode;
	}

	inline bool IsInPlayMode()
	{
		return Internal::currentMode == Mode::Play;
	}

	/// @brief Program has entered play mode and is now running the game
	inline void PlayModeEntered()
	{
		Internal::enteredPlayMode = false;
	}

	inline bool HasEnteredPlayMode()
	{
		return Internal::enteredPlayMode;
	}
}

namespace EngineSettings
{
	constexpr int WindowWidth = 1280;
	constexpr int WindowHeight = 720;
}
