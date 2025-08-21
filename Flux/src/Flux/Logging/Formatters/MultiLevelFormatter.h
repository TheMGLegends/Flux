#pragma once

#include <array>
#include <spdlog/pattern_formatter.h>

namespace Flux
{
	class MultiLevelFormatter : public spdlog::formatter
	{
	public:
		MultiLevelFormatter();
		MultiLevelFormatter(const std::array<std::string, spdlog::level::n_levels>& _patterns);

		void SetPattern(const std::string& pattern, spdlog::level::level_enum level);
		const std::string& GetPattern(spdlog::level::level_enum level) const;

		virtual void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override;
		virtual std::unique_ptr<spdlog::formatter> clone() const override;

	private:
		void InitialisePatternFormatters();

	private:
		std::array<std::string, spdlog::level::n_levels> patterns;
		std::array<std::unique_ptr<spdlog::pattern_formatter>, spdlog::level::n_levels> patternFormatters;
	};
}