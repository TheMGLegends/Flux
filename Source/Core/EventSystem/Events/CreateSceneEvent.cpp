#include "CreateSceneEvent.h"

namespace Flux
{
	CreateSceneEvent::CreateSceneEvent(const std::string& _sceneName, const std::filesystem::path& _scenePath) : sceneName(_sceneName), scenePath(_scenePath)
	{
	}

	CreateSceneEvent::~CreateSceneEvent() = default;
}
