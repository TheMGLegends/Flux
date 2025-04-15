#pragma once

namespace Flux::EngineConfig
{
	namespace Internal
	{
		extern int windowWidth;
		extern int windowHeight;
	}

	extern const float ASPECT_RATIO;
	extern const bool DIRECT_INPUT_DISABLED;

	void SetWindowWidth(int _windowWidth);
	int GetWindowWidth();

	void SetWindowHeight(int _windowHeight);
	int GetWindowHeight();

	int& WindowWidthRef();
	int& WindowHeightRef();
}

