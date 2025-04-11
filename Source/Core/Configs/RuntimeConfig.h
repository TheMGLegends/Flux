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
		extern Mode currentMode;
		extern bool enteredPlayMode;
		extern bool isPaused;
	}

	void SetMode(Mode newMode);
	bool IsInEditorMode();
	bool IsInPlayMode();

	void PlayModeEntered();
	bool HasEnteredPlayMode();
	void SetPlayModeEntered(bool entered);

	bool IsPaused();
	void TogglePause();
}

