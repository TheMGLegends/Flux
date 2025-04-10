#include "LoadSceneEvent.h"

namespace Flux
{
	LoadSceneEvent::LoadSceneEvent(const std::filesystem::path& _scenePath) : scenePath(_scenePath)
	{
	}

	LoadSceneEvent::~LoadSceneEvent()
	{
	}
}
