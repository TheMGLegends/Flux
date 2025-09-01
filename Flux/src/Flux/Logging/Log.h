#pragma once

#include "Flux/Core.h"

#if defined(FLUX_DEBUG) || defined(FLUX_DEVELOPMENT)
	#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#elif defined(FLUX_RELEASE)
	#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_OFF
#endif
#include <spdlog/spdlog.h>

namespace Flux
{
	/// <summary>
	/// Wrapper class for logging system provided by spdlog
	/// </summary>
	class Log
	{
	public:
		Log() = delete;
		Log(const Log&) = delete;
		Log(Log&&) = delete;
		Log& operator=(const Log&) = delete;
		~Log() = delete;

		static void Initialise();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger();
		static std::shared_ptr<spdlog::logger>& GetClientLogger();
	};
}

#if defined(FLUX_DEBUG) || defined(FLUX_DEVELOPMENT)

// INFO: Core Logger Macros
#define FLUX_CORE_TRACE(...)	SPDLOG_LOGGER_TRACE(Flux::Log::GetCoreLogger(), __VA_ARGS__)
#define FLUX_CORE_DEBUG(...)	SPDLOG_LOGGER_DEBUG(Flux::Log::GetCoreLogger(), __VA_ARGS__)
#define FLUX_CORE_INFO(...)		SPDLOG_LOGGER_INFO(Flux::Log::GetCoreLogger(), __VA_ARGS__)
#define FLUX_CORE_WARN(...)		SPDLOG_LOGGER_WARN(Flux::Log::GetCoreLogger(), __VA_ARGS__)
#define FLUX_CORE_ERROR(...)	SPDLOG_LOGGER_ERROR(Flux::Log::GetCoreLogger(), __VA_ARGS__)
#define FLUX_CORE_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(Flux::Log::GetCoreLogger(), __VA_ARGS__)

// INFO: Client Logger Macros
#define FLUX_INFO(...)			::Flux::Log::GetClientLogger()->info(__VA_ARGS__)
#define FLUX_WARN(...)			::Flux::Log::GetClientLogger()->warn(__VA_ARGS__)
#define FLUX_ERROR(...)			::Flux::Log::GetClientLogger()->error(__VA_ARGS__)

#elif defined(FLUX_RELEASE) // INFO: Logging Macros Scraped in Release Builds

// INFO: Core Logger Macros
#define FLUX_CORE_TRACE(...)	((void)0)
#define FLUX_CORE_DEBUG(...)	((void)0)
#define FLUX_CORE_INFO(...)     ((void)0)
#define FLUX_CORE_WARN(...)     ((void)0)
#define FLUX_CORE_ERROR(...)    ((void)0)
#define FLUX_CORE_CRITICAL(...) ((void)0)

// INFO: Client Logger Macros
#define FLUX_INFO(...)          ((void)0)
#define FLUX_WARN(...)          ((void)0)
#define FLUX_ERROR(...)         ((void)0)

#endif