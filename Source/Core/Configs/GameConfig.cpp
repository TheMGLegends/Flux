#include "GameConfig.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Core/Configs/RendererConfig.h"

#include "Core/Debug/Debug.h"
#include "Core/Debug/FrameRateMonitor.h"

#include "Engine/Entities/GameObjects/SceneViewCamera.h"

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

	void SerializeGameConfig(SceneViewCamera* sceneViewCamera)
	{
		using namespace Internal;

		nlohmann::flux_json json;
		json["StarterSceneName"] = starterSceneName;
		json["FrameCounterActive"] = FrameRateMonitor::IsActive();
		json["VSyncEnabled"] = RendererConfig::IsVSyncEnabled();

		if (sceneViewCamera) { sceneViewCamera->SerializeEditorCamera(json); }

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

	void DeserializeGameConfig(SceneViewCamera* sceneViewCamera)
	{
		using namespace Internal;

		std::ifstream file("GameSettings.json");

		if (file.is_open())
		{
			nlohmann::flux_json json = nlohmann::json::parse(file);
			starterSceneName = json["StarterSceneName"].get<std::string>();
			FrameRateMonitor::SetIsActive(json["FrameCounterActive"].get<bool>());
			RendererConfig::SetVSyncEnabled(json["VSyncEnabled"].get<bool>());
			if (sceneViewCamera) { sceneViewCamera->DeserializeEditorCamera(json); }
		}
		else
		{
			Debug::LogError("GameConfig::DeserializeGameConfig() - Failed to open GameSettings.json");
			SerializeGameConfig(sceneViewCamera);
		}
	}
}
