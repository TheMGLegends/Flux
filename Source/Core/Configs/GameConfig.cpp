#include "GameConfig.h"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Core/Configs/EditorConfig.h"
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
		json["TransformOperation"] = EditorConfig::GetCurrentTransformOperation();
		json["TransformMode"] = EditorConfig::GetTransformMode();

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

			if (json.contains("StarterSceneName"))
			{
				starterSceneName = json["StarterSceneName"].get<std::string>();
			}

			if (json.contains("FrameCounterActive"))
			{
				FrameRateMonitor::SetIsActive(json["FrameCounterActive"].get<bool>());
			}

			if (json.contains("VSyncEnabled"))
			{
				RendererConfig::SetVSyncEnabled(json["VSyncEnabled"].get<bool>());
			}

			if (json.contains("TransformOperation"))
			{
				EditorConfig::SetCurretTransformOperation(json["TransformOperation"].get<int>());
			}

			if (json.contains("TransformMode"))
			{
				EditorConfig::SetTransformMode(static_cast<ImGuizmo::MODE>(json["TransformMode"].get<int>()));
			}

			if (sceneViewCamera) { sceneViewCamera->DeserializeEditorCamera(json); }
		}
		else
		{
			Debug::LogError("GameConfig::DeserializeGameConfig() - Failed to open GameSettings.json");
			SerializeGameConfig(sceneViewCamera);
		}
	}
}
