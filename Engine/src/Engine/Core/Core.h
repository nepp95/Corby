#pragma once

#include <memory>

#ifdef ENG_PLATFORM_WINDOWS
#if ENG_DYNAMIC_LINK
#ifdef ENG_BUILD_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif
#else
#define ENGINE_API
#endif
#else
#error Engine only supports Windows!
#endif

#ifdef ENG_DEBUG
#define ENG_ENABLE_ASSERTS
#endif

#ifdef ENG_ENABLE_ASSERTS
#define ENG_ASSERT(x, ...) { if(!(x)) { ENG_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }}
#define ENG_CORE_ASSERT(x, ...) { if(!(x)) { ENG_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
#define ENG_ASSERT(x, ...)
#define ENG_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define ENG_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Engine {
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;
}