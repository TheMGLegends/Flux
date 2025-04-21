#pragma once

namespace Flux::RendererConfig
{
	namespace Internal
	{
		extern bool vsyncEnabled;
	}

	void ToggleVSync();
	void SetVSyncEnabled(bool _vsyncEnabled);
	bool IsVSyncEnabled();
}

