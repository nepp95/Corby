#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Engine {
	class ENGINE_API Log {
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_coreLogger;
		static std::shared_ptr<spdlog::logger> s_clientLogger;
	};
}

#define ENGINE_CORE_TRACE(...)		::Engine::Log::getCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_CORE_INFO(...)		::Engine::Log::getCoreLogger()->info(__VA_ARGS__)
#define ENGINE_CORE_WARN(...)		::Engine::Log::getCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_CORE_ERROR(...)		::Engine::Log::getCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CORE_CRITICAL(...)	::Engine::Log::getCoreLogger()->critical(__VA_ARGS__)

#define ENGINE_TRACE(...)			::Engine::Log::getClientLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...)			::Engine::Log::getClientLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...)			::Engine::Log::getClientLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...)			::Engine::Log::getClientLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...)		::Engine::Log::getClientLogger()->critical(__VA_ARGS__)