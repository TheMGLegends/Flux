#pragma once

#include <string>

namespace Flux::GameConfig
{
	namespace Internal
	{
		extern std::string starterSceneName;
	}

	void SetStarterSceneName(std::string_view _starterSceneName);
	const std::string& GetStarterSceneName();

	void SerializeGameConfig();
	void DeserializeGameConfig();
}

