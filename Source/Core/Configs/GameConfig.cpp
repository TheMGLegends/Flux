#include "GameConfig.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Core/Configs/RendererConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Debug/FrameRateMonitor.h"

namespace Flux::GameConfig
{
	namespace Internal
	{
		std::string starterSceneName = "";
	}

	void SetStarterSceneName(std::string_view _starterSceneName)
	{
		using namespace Internal;
		starterSceneName = _starterSceneName;
	}

	const std::string& GetStarterSceneName()
	{
		using namespace Internal;
		return starterSceneName;
	}

	void SerializeGameConfig()
	{
		using namespace Internal;

		nlohmann::json json;
		json["StarterSceneName"] = starterSceneName;
		json["FrameCounterActive"] = FrameRateMonitor::IsActive();
		json["VSyncEnabled"] = RendererConfig::IsVSyncEnabled();

		std::ofstream file("GameSettings.json");
		if (file.is_open())
		{
			file << json.dump(4);
		}
		else
		{
			Debug::LogError("GameConfig::SerializeGameConfig() - Failed to open GameSettings.json");
		}
	}

	void DeserializeGameConfig()
	{
		using namespace Internal;

		std::ifstream file("GameSettings.json");

		if (file.is_open())
		{
			nlohmann::json json = nlohmann::json::parse(file);
			starterSceneName = json["StarterSceneName"].get<std::string>();
			FrameRateMonitor::SetIsActive(json["FrameCounterActive"].get<bool>());
			RendererConfig::SetVSyncEnabled(json["VSyncEnabled"].get<bool>());
		}
		else
		{
			Debug::LogError("GameConfig::DeserializeGameConfig() - Failed to open GameSettings.json");
			SerializeGameConfig();
		}
	}
}
