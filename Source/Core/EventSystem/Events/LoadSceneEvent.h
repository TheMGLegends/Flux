#pragma once

#include "Event.h"

#include <filesystem>

namespace Flux
{
	class LoadSceneEvent : public Event
	{
	public:
		LoadSceneEvent(const std::filesystem::path& _scenePath);
		virtual ~LoadSceneEvent() override;

	public:
		std::filesystem::path scenePath;
	};
}

