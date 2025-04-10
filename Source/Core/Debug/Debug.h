#pragma once

#include <string>

namespace Flux::Debug
{
	void Log(const std::string& message);
	void LogWarning(const std::string& message);
	void LogError(const std::string& message);
}

