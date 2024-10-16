#pragma once


#if defined(_WIN32) || defined(_WIN64)
	#define MN_PLATFORM_WINDOWS
#elif defined(__linux__)
	#define MN_PLATFORM_LINUX
#endif


#ifdef NDEBUG
	#define MN_DEBUG 0
#else
	#define MN_DEBUG 1
#endif


#include <cstdint>
#include <memory>
#include <cstdarg>
#include "log.h"


namespace minik {

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}



