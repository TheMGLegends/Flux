#include "FluxPCH.h" // INFO: Precompiled Header

#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include "Formatters/MultiLevelFormatter.h"

namespace Flux
{
	// INFO: Internal Linkage to suppress warning C4251 from FLUX_API macro
	static std::shared_ptr<spdlog::logger> coreLogger;
	static std::shared_ptr<spdlog::logger> clientLogger;

	void Log::Initialise()
	{
		// INFO: Core Logger Creation
		auto coreSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		FLUX_ASSERT(coreSink != nullptr, "Core Sink failed to initialise!"); // INFO: Using assert because you cannot log without a logger
		coreSink->set_formatter(std::make_unique<MultiLevelFormatter>());

		coreLogger = std::make_shared<spdlog::logger>("FLUX", coreSink);
		FLUX_ASSERT(coreLogger != nullptr, "Core Logger failed to initialise!");
		spdlog::register_logger(coreLogger);
		coreLogger->set_level(spdlog::level::trace);

		// INFO: Client Logger Creation
		clientLogger = spdlog::stdout_color_mt("APP");
		FLUX_ASSERT(clientLogger != nullptr, "Client Logger failed to initialise!");
		clientLogger->set_level(spdlog::level::trace);
		clientLogger->set_pattern("[%T]%^ %n: %v%$"); // Time, Color-Begin, Logger Name, Message, Color-End
	}

	std::shared_ptr<spdlog::logger>& Log::GetCoreLogger()
	{
		return coreLogger;
	}

	std::shared_ptr<spdlog::logger>& Log::GetClientLogger()
	{
		return clientLogger;
	}
}