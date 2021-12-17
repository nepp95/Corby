#pragma once

#include <memory>

#include "Engine/Core/PlatformDetection.h"

#ifdef ENG_DEBUG
#if defined(ENG_PLATFORM_WINDOWS)
#define ENG_DEBUGBREAK() __debugbreak()
#elif defined(ENG_PLATFORM_LINUX)
#include <signal.h>
#define ENG_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform does not support debugbreak yet!"
#endif
#define ENG_ENABLE_ASSERTS
#else
#define ENG_DEBUGREAK()
#endif

#define ENG_PROFILE
#ifdef ENG_PROFILE
#define ENG_PROFILE_BEGIN_SESSION(name, filepath) ::Engine::Instrumentor::get().beginSession(name, filepath)
#define ENG_PROFILE_END_SESSION() ::Engine::Instrumentor::get().endSession()
#define ENG_PROFILE_SCOPE(name) ::Engine::InstrumentationTimer timer##__LINE__(name);
#define ENG_PROFILE_FUNCTION() ENG_PROFILE_SCOPE(__FUNCSIG__)
#else
#define ENG_PROFILE_BEGIN_SESSION(name, filepath)
#define ENG_PROFILE_END_SESSION()
#define ENG_PROFILE_SCOPE(name)
#define ENG_PROFILE_FUNCTION()
#endif

#define BIT(x) (1 << x)
#define ENG_EXPAND_MACRO(x) x
#define ENG_STRINGIFY_MACRO(x) #x
#define ENG_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Engine {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"
