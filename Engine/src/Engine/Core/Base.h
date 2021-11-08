#pragma once

#include <memory>

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
#ifdef _WIN64
	/* Windows x64  */
#define ENG_PLATFORM_WINDOWS
#else
	/* Windows x86 */
#error "x86 Builds are not supported!"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
/* TARGET_OS_MAC exists on all the platforms
 * so we must check all of them (in this order)
 * to ensure that we're running on MAC
 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define ENG_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define ENG_PLATFORM_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#define ENG_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define ENG_PLATFORM_LINUX
#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
#error "Unknown platform!"
#endif // End of platform detection

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

// TODO: Make this macro able to take in no arguments except condition
#ifdef ENG_ENABLE_ASSERTS
#define ENG_ASSERT(x, ...) { if(!(x)) { ENG_ERROR("Assertion Failed: {0}", __VA_ARGS__); ENG_DEBUGBREAK(); } }
#define ENG_CORE_ASSERT(x, ...) { if(!(x)) { ENG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); ENG_DEBUGBREAK(); } }
#else
#define ENG_ASSERT(x, ...)
#define ENG_CORE_ASSERT(x, ...)
#endif

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

#define ENG_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

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