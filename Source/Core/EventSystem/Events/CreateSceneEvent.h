#pragma once

#include "Event.h"

#include <filesystem>
#include <string>

namespace Flux
{
	class CreateSceneEvent : public Event
	{
	public:
		explicit CreateSceneEvent(const std::string& _sceneName, const std::filesystem::path& _scenePath);
		~CreateSceneEvent() override;

	public:
		std::string sceneName;
		std::filesystem::path scenePath;
	};
}

