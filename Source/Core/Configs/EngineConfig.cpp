#include "EngineConfig.h"

namespace Flux::EngineConfig
{
	namespace Internal
	{
		int windowWidth = 1920;
		int windowHeight = 1080;
	}

	const float ASPECT_RATIO = 16.0f / 9.0f;
	const bool DIRECT_INPUT_DISABLED = true;

	void SetWindowWidth(int _windowWidth)
	{
		using namespace Internal;
		windowWidth = _windowWidth;
	}

	int GetWindowWidth()
	{
		using namespace Internal;
		return windowWidth;
	}

	void SetWindowHeight(int _windowHeight)
	{
		using namespace Internal;
		windowHeight = _windowHeight;
	}

	int GetWindowHeight()
	{
		using namespace Internal;
		return windowHeight;
	}

	int& WindowWidthRef()
	{
		using namespace Internal;
		return windowWidth;
	}
	int& WindowHeightRef()
	{
		using namespace Internal;
		return windowHeight;
	}
}
