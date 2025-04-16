#include "LoadSceneEvent.h"

namespace Flux
{
	LoadSceneEvent::LoadSceneEvent(const std::filesystem::path& _scenePath, bool _stayInPlayMode) : scenePath(_scenePath), 
																									stayInPlayMode(_stayInPlayMode)
	{
	}

	LoadSceneEvent::~LoadSceneEvent()
	{
	}
}
