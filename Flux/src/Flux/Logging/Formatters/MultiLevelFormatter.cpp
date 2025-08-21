#include "FluxPCH.h" // INFO: Precompiled Header

#include "MultiLevelFormatter.h"

namespace Flux
{
	MultiLevelFormatter::MultiLevelFormatter()
	{
		// INFO: Store Default Patterns for each Log Level
		patterns[spdlog::level::trace] = "[%T]%^ %n: %v%$ [%@]"; // Time, Color-Begin, Logger Name, Message, Color-End, Detailed File and Line
		patterns[spdlog::level::debug] = "[%T]%^ %n: %v%$"; // Time, Color-Begin, Logger Name, Message, Color-End
		patterns[spdlog::level::info] = "[%T]%^ %n: %v%$ [%!]"; // Time, Color-Begin, Logger Name, Message, Color-End, Function Name
		patterns[spdlog::level::warn] = "[%T]%^ %n: %v%$ [%s:%#]"; // Time, Color-Begin, Logger Name, Message, Color-End, File and Line
		patterns[spdlog::level::err] = "[%T]%^ %n: %v%$ [%s:%#]"; // Time, Color-Begin, Logger Name, Message, Color-End, File and Line
		patterns[spdlog::level::critical] = "[%T]%^ %n: %v%$ [%s:%#]"; // Time, Color-Begin, Logger Name, Message, Color-End, File and Line
		patterns[spdlog::level::off] = ""; // No output for off level

		InitialisePatternFormatters();
	}

	MultiLevelFormatter::MultiLevelFormatter(const std::array<std::string, spdlog::level::n_levels>& _patterns) : patterns(_patterns)
	{
		InitialisePatternFormatters();
	}

	void MultiLevelFormatter::SetPattern(const std::string& pattern, spdlog::level::level_enum level)
	{
		FLUX_CORE_ASSERT(level < spdlog::level::n_levels, "Invalid log level provided for pattern setting!");

		// INFO: Set the Provided Pattern for the Specific Log Level
		patterns[level] = pattern;
		patternFormatters[level]->set_pattern(patterns[level]);
	}

	const std::string& MultiLevelFormatter::GetPattern(spdlog::level::level_enum level) const
	{
		FLUX_CORE_ASSERT(level < spdlog::level::n_levels, "Invalid log level provided for pattern retrieval!");
		return patterns[level];
	}

	void MultiLevelFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest)
	{
		// INFO: Format the message using the pattern formatter for the specific log level
		if (auto& patternFormatter = patternFormatters[msg.level]; patternFormatter)
		{
			patternFormatter->format(msg, dest);
		}
	}

	std::unique_ptr<spdlog::formatter> MultiLevelFormatter::clone() const
	{
		// INFO: Create a Copy using the Current Patterns
		return spdlog::details::make_unique<MultiLevelFormatter>(patterns);
	}

	void MultiLevelFormatter::InitialisePatternFormatters()
	{
		for (size_t i = 0; i < spdlog::level::n_levels; ++i)
		{
			patternFormatters[i] = std::make_unique<spdlog::pattern_formatter>(patterns[i]);
		}
	}
}