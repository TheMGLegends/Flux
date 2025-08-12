#include "FluxPCH.h" // INFO: Precompiled Header

#include "CoreFormatter.h"

namespace Flux
{
	CoreFormatter::CoreFormatter()
	{
		// INFO: Default Patterns for each Log Level (Debug & Info do not log file and line information)
		patternFormatters[spdlog::level::trace] = std::make_unique<spdlog::pattern_formatter>("[%T]%^ %n: %v%$ [%@]"); // Time, Color-Begin, Logger Name, Message, Color-End, Detailed File and Line
		patternFormatters[spdlog::level::debug] = std::make_unique<spdlog::pattern_formatter>("[%T]%^ %n: %v%$"); // Time, Color-Begin, Logger Name, Message, Color-End
		patternFormatters[spdlog::level::info] = std::make_unique<spdlog::pattern_formatter>("[%T]%^ %n: %v%$ [%!]"); // Time, Color-Begin, Logger Name, Message, Color-End, Function Name
		patternFormatters[spdlog::level::warn] = std::make_unique<spdlog::pattern_formatter>("[%T]%^ %n: %v%$ [%s:%#]"); // Time, Color-Begin, Logger Name, Message, Color-End, File and Line
		patternFormatters[spdlog::level::err] = std::make_unique<spdlog::pattern_formatter>("[%T]%^ %n: %v%$ [%s:%#]");
		patternFormatters[spdlog::level::critical] = std::make_unique<spdlog::pattern_formatter>("[%T]%^ %n: %v%$ [%s:%#]");
		patternFormatters[spdlog::level::off] = std::make_unique<spdlog::pattern_formatter>("");
	}

	CoreFormatter::CoreFormatter(std::array<std::string_view, spdlog::level::n_levels> patterns)
	{
		// INFO: Create a Custom Pattern for each Log Level
		for (size_t i = 0; i < spdlog::level::n_levels; ++i)
		{
			std::string pattern(patterns[i]);
			patternFormatters[i] = std::make_unique<spdlog::pattern_formatter>(pattern);
		}
	}

	void CoreFormatter::SetPattern(std::string_view patternView, spdlog::level::level_enum level)
	{
		// INFO: Ignore Patterns for Invalid Log Levels
		if (level == spdlog::level::n_levels) { return; }

		std::string pattern(patternView);
		patternFormatters[level]->set_pattern(pattern);
	}

	void CoreFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest)
	{
		// INFO: Format the message using the pattern formatter for the specific log level
		if (auto& patternFormater = patternFormatters[msg.level]; patternFormater)
		{
			patternFormater->format(msg, dest);
		}
	}

	std::unique_ptr<spdlog::formatter> CoreFormatter::clone() const
	{
		return spdlog::details::make_unique<CoreFormatter>();
	}
}