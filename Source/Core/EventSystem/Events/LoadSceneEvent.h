#pragma once

#include "Event.h"

#include <filesystem>

namespace Flux
{
	class LoadSceneEvent : public Event
	{
	public:
		LoadSceneEvent(const std::filesystem::path& _scenePath, bool _stayInPlayMode = false);
		~LoadSceneEvent() override;

	public:
		std::filesystem::path scenePath;
		bool stayInPlayMode;
	};
}

