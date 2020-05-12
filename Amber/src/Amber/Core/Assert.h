#pragma once

#ifdef AB_ENABLE_ASSERTS

    #define AB_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { AB_ERROR("Assertion Failed"); AB_DEBUGBREAK(); } }
    #define AB_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { AB_ERROR("Assertion Failed: {0}", __VA_ARGS__); AB_DEBUGBREAK(); } }

    #define AB_CORE_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { AB_CORE_ERROR("Assertion Failed"); AB_DEBUGBREAK(); } }
    #define AB_CORE_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { AB_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); AB_DEBUGBREAK(); } }

    #define AB_ASSERT_RESOLVE(arg1, arg2, macro, ...) macro
    #define AB_GET_ASSERT_MACRO(...) AB_EXPAND_VARGS(AB_ASSERT_RESOLVE(__VA_ARGS__, AB_ASSERT_MESSAGE, AB_ASSERT_NO_MESSAGE))
    #define AB_GET_CORE_ASSERT_MACRO(...) AB_EXPAND_VARGS(AB_ASSERT_RESOLVE(__VA_ARGS__, AB_CORE_ASSERT_MESSAGE, AB_CORE_ASSERT_NO_MESSAGE))

    #define AB_ASSERT(...) AB_EXPAND_VARGS(AB_GET_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__))
    #define AB_CORE_ASSERT(...) AB_EXPAND_VARGS(AB_GET_CORE_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__))

#else
    #define AB_ASSERT(x, ...)
    #define AB_CORE_ASSERT(x, ...)
#endif