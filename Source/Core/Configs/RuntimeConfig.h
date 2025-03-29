#pragma once

namespace Flux::RuntimeConfig
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
		inline bool isPaused = false;
	}

	inline void SetMode(Mode newMode)
	{
		using namespace Internal;

		if (currentMode == Mode::Editor && newMode == Mode::Play) { enteredPlayMode = true; }

		currentMode = newMode;
	}

	inline bool IsInEditorMode()
	{
		using namespace Internal;
		return currentMode == Mode::Editor;
	}

	inline bool IsInPlayMode()
	{
		using namespace Internal;
		return currentMode == Mode::Play;
	}

	/// @brief Program has entered play mode and is now running the game
	inline void PlayModeEntered()
	{
		using namespace Internal;
		enteredPlayMode = false;
	}

	inline bool HasEnteredPlayMode()
	{
		using namespace Internal;
		return enteredPlayMode;
	}

	inline bool IsPaused()
	{
		using namespace Internal;
		return isPaused;
	}

	inline void TogglePause()
	{
		using namespace Internal;
		isPaused = !isPaused;
	}
}

