#pragma once

#include "Engine/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Engine {
	class Log {
	public:
		static void init();

		inline static Ref<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
		inline static Ref<spdlog::logger>& getClientLogger() { return s_clientLogger; }
	private:
		static Ref<spdlog::logger> s_coreLogger;
		static Ref<spdlog::logger> s_clientLogger;
	};
}

#define ENG_CORE_TRACE(...)		::Engine::Log::getCoreLogger()->trace(__VA_ARGS__)
#define ENG_CORE_INFO(...)		::Engine::Log::getCoreLogger()->info(__VA_ARGS__)
#define ENG_CORE_WARN(...)		::Engine::Log::getCoreLogger()->warn(__VA_ARGS__)
#define ENG_CORE_ERROR(...)		::Engine::Log::getCoreLogger()->error(__VA_ARGS__)
#define ENG_CORE_CRITICAL(...)	::Engine::Log::getCoreLogger()->critical(__VA_ARGS__)

#define ENG_TRACE(...)			::Engine::Log::getClientLogger()->trace(__VA_ARGS__)
#define ENG_INFO(...)			::Engine::Log::getClientLogger()->info(__VA_ARGS__)
#define ENG_WARN(...)			::Engine::Log::getClientLogger()->warn(__VA_ARGS__)
#define ENG_ERROR(...)			::Engine::Log::getClientLogger()->error(__VA_ARGS__)
#define ENG_CRITICAL(...)		::Engine::Log::getClientLogger()->critical(__VA_ARGS__)