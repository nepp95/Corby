#pragma once

#include "Engine/Core/Base.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Engine
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_coreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_clientLogger; }
	private:
		static Ref<spdlog::logger> s_coreLogger;
		static Ref<spdlog::logger> s_clientLogger;
	};
}

#define ENG_CORE_TRACE(...)		::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENG_CORE_INFO(...)		::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENG_CORE_WARN(...)		::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENG_CORE_ERROR(...)		::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENG_CORE_CRITICAL(...)	::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define ENG_TRACE(...)			::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ENG_INFO(...)			::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ENG_WARN(...)			::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ENG_ERROR(...)			::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ENG_CRITICAL(...)		::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)
