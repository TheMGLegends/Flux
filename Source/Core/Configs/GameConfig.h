#pragma once

#include <string>

namespace Flux
{
	class SceneViewCamera;

	namespace GameConfig
	{
		namespace Internal
		{
			extern std::string starterSceneName;
		}

		void SetStarterSceneName(std::string_view _starterSceneName);
		const std::string& GetStarterSceneName();

		void SerializeGameConfig(SceneViewCamera* sceneViewCamera);
		void DeserializeGameConfig(SceneViewCamera* sceneViewCamera);
	}
}

