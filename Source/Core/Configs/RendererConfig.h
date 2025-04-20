#pragma once

namespace Flux::RendererConfig
{
	namespace Internal
	{
		extern bool vsyncEnabled;
	}

	void ToggleVSync();
	bool IsVSyncEnabled();
}

