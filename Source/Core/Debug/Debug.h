#pragma once

#include <string>
#include <iostream>

namespace Debug
{
	void Log(const std::string& message)
	{
		std::cout << message << std::endl;
	}

	void LogWarning(const std::string& message)
	{
		// INFO: Set color to bold yellow
		std::cout << "\033[1;33;1m";

		std::cout << "Warning: " + message << std::endl;

		// INFO: Reset color to default
		std::cout << "\033[0m";
	}

	void LogError(const std::string& message)
	{
		// INFO: Set color to bold red
		std::cout << "\033[1;31;1m";

		std::cout << "Error: " + message << std::endl;

		// INFO: Reset color to default
		std::cout << "\033[0m";
	}
}