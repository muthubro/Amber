#pragma once

#include "Amber/Core/Core.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Amber
{

class Log
{
public:
	static void Init();

	static std::shared_ptr<spdlog::logger> GetCoreLogger() { return s_CoreLogger; }
	static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }

private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

}

#define AB_LOG_INIT() ::Amber::Log::Init()

// Core logging
#define AB_CORE_CRITICAL(...)  ::Amber::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define AB_CORE_ERROR(...)     ::Amber::Log::GetCoreLogger()->error(__VA_ARGS__)
#define AB_CORE_WARN(...)      ::Amber::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define AB_CORE_INFO(...)      ::Amber::Log::GetCoreLogger()->info(__VA_ARGS__)
#define AB_CORE_TRACE(...)     ::Amber::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Client logging
#define AB_CRITICAL(...)       ::Amber::Log::GetClientLogger()->critical(__VA_ARGS__)
#define AB_ERROR(...)          ::Amber::Log::GetClientLogger()->error(__VA_ARGS__)
#define AB_WARN(...)           ::Amber::Log::GetClientLogger()->warn(__VA_ARGS__)
#define AB_INFO(...)           ::Amber::Log::GetClientLogger()->info(__VA_ARGS__)
#define AB_TRACE(...)          ::Amber::Log::GetClientLogger()->trace(__VA_ARGS__)
