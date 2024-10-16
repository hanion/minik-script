#pragma once

#include <iostream>

#if MN_DEBUG
	#define MN_LOG(fmt, ...)            \
		do {                            \
			std::cout << "[DEBUG] ";    \
			printf(fmt, ##__VA_ARGS__); \
			std::cout << std::endl;     \
		} while (0)
#else
	#define MN_LOG(fmt, ...)
#endif

