#include "RendererConfig.h"

namespace Flux::RendererConfig
{
	namespace Internal
	{
		bool vsyncEnabled = false;
	}

	void ToggleVSync()
	{
		using namespace Internal;
		vsyncEnabled = !vsyncEnabled;
	}

	bool IsVSyncEnabled()
	{
		using namespace Internal;
		return vsyncEnabled;
	}
}
