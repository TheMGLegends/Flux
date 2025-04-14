#pragma once

#include <string>

namespace Flux::GameConfig
{
	namespace Internal
	{
		extern std::string starterSceneName;
	}

	void SetStarterSceneName(const std::string& _starterSceneName);
	const std::string& GetStarterSceneName();

	void SerializeGameConfig();
	void DeserializeGameConfig();
}

