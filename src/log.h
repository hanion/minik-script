#pragma once

#include <cstdarg>
#include <iostream>

#define COLOR_RESET "\033[0m"
#define COLOR_RED   "\033[1;31m"
#define COLOR_GREEN "\033[32m"

inline void mn_print_colored(const char* color, const char* prefix, const char* fmt, ...) {
	std::cout << color << prefix;
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	std::cout << COLOR_RESET << std::endl;
}


#if MN_DEBUG
	#define MN_LOG(fmt, ...) mn_print_colored(COLOR_RESET, "[DEBUG] ", fmt, ##__VA_ARGS__)
	#define MN_ERROR(fmt, ...) mn_print_colored(COLOR_RED, "[ERROR] ", fmt, ##__VA_ARGS__)
#else
	#define MN_LOG(fmt, ...)
	#define MN_ERROR(fmt, ...)
#endif

