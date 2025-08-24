#include "FluxPCH.h"

#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include "Formatters/MultiLevelFormatter.h"

namespace Flux
{
	void Log::Initialise()
	{
		static bool isInitialised = false;

		if (!isInitialised)
		{
			// INFO: Core Logger Creation
			auto coreSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			FLUX_MINIMAL_ASSERT(coreSink != nullptr);
			coreSink->set_formatter(std::make_unique<MultiLevelFormatter>());

			auto& coreLogger = GetCoreLogger();
			coreLogger = std::make_shared<spdlog::logger>("FLUX", coreSink);
			FLUX_MINIMAL_ASSERT(coreLogger != nullptr);
			spdlog::register_logger(coreLogger);
			coreLogger->set_level(spdlog::level::trace);

			// INFO: Client Logger Creation
			auto& clientLogger = GetClientLogger();
			clientLogger = spdlog::stdout_color_mt("APP");
			FLUX_CORE_ASSERT(clientLogger != nullptr, "Client Logger failed to initialise!");
			clientLogger->set_level(spdlog::level::trace);
			clientLogger->set_pattern("[%T]%^ %n: %v%$"); // Time, Color-Begin, Logger Name, Message, Color-End

			isInitialised = true;
		}
	}

	std::shared_ptr<spdlog::logger>& Log::GetCoreLogger()
	{
		static std::shared_ptr<spdlog::logger> coreLogger;
		return coreLogger;
	}

	std::shared_ptr<spdlog::logger>& Log::GetClientLogger()
	{
		static std::shared_ptr<spdlog::logger> clientLogger;
		return clientLogger;
	}
}