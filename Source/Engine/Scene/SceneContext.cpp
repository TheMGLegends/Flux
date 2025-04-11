#include "SceneContext.h"

#include "Core/EventSystem/EventDispatcher.h"
#include "Core/EventSystem/Events/LoadSceneEvent.h"
#include "Core/Renderer/AssetHandler.h"

namespace Flux
{
	Scene* SceneContext::scene = nullptr;

	void SceneContext::LoadScene(const std::string& sceneName)
	{
		std::filesystem::path scenePath = AssetHandler::GetScenePath(sceneName);

		if (scenePath.empty())
		{
			Debug::LogError("SceneContext::LoadScene - Failed to find scene path. Scene Name: " + sceneName);
			return;
		}

		EventDispatcher::QueueEvent(EventType::LoadScene, std::make_shared<LoadSceneEvent>(scenePath, true));
	}
}
