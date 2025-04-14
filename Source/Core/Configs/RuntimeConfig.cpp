#include "RuntimeConfig.h"

namespace Flux::RuntimeConfig
{
	namespace Internal
	{
		Mode currentMode = Mode::Editor;
		bool enteredPlayMode = false;
		bool isPaused = false;
		bool isStandalone = false;
	}

	void SetMode(Mode newMode)
	{
		using namespace Internal;

		if (currentMode == Mode::Editor && newMode == Mode::Play) { enteredPlayMode = true; }
		currentMode = newMode;
	}

	bool IsInEditorMode()
	{
		using namespace Internal;
		return currentMode == Mode::Editor;
	}

	bool IsInPlayMode()
	{
		using namespace Internal;
		return currentMode == Mode::Play;
	}

	void PlayModeEntered()
	{
		using namespace Internal;
		enteredPlayMode = false;
	}

	bool HasEnteredPlayMode()
	{
		using namespace Internal;
		return enteredPlayMode;
	}

	void SetPlayModeEntered(bool entered)
	{
		using namespace Internal;
		enteredPlayMode = entered;
	}

	bool IsPaused()
	{
		using namespace Internal;
		return isPaused;
	}

	void TogglePause()
	{
		using namespace Internal;
		isPaused = !isPaused;
	}

	bool IsStandalone()
	{
		using namespace Internal;
		return isStandalone;
	}

	void EnableStandalone()
	{
		using namespace Internal;
		isStandalone = true;
		SetMode(Mode::Play);
	}
}