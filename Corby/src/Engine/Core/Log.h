#pragma once

#include "Engine/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternio)
{
	return os << glm::to_string(quaternio);
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
