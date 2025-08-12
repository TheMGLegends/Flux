#pragma once

#include <array>

#include "spdlog/pattern_formatter.h"

namespace Flux
{
	class CoreFormatter : public spdlog::formatter
	{
	public:
		CoreFormatter();
		CoreFormatter(std::array<std::string_view, spdlog::level::n_levels> patterns);

		void SetPattern(std::string_view patternView, spdlog::level::level_enum level);

		virtual void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override;
		virtual std::unique_ptr<spdlog::formatter> clone() const override;

	private:
		std::array<std::unique_ptr<spdlog::pattern_formatter>, spdlog::level::n_levels> patternFormatters;
	};
}