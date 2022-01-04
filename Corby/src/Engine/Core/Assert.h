#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"

#include <filesystem>

#ifdef ENG_ENABLE_ASSERTS

#define ENG_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { ENG##type##ERROR(msg, __VA_ARGS__); ENG_DEBUGBREAK(); } }
#define ENG_INTERNAL_ASSERT_WITH_MSG(type, check, ...) ENG_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define ENG_INTERNAL_ASSERT_NO_MSG(type, check) ENG_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", ENG_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define ENG_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define ENG_INTERNAL_ASSERT_GET_MACRO(...) ENG_EXPAND_MACRO(ENG_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, ENG_INTERNAL_ASSERT_WITH_MSG, ENG_INTERNAL_ASSERT_NO_MSG))

#define ENG_ASSERT(...) ENG_EXPAND_MACRO(ENG_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define ENG_CORE_ASSERT(...) ENG_EXPAND_MACRO(ENG_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define ENG_ASSERT(...)
#define ENG_CORE_ASSERT(...)
#endif
