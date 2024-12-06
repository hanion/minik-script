#pragma once

#include <cstdarg>
#include <iostream>

#define COLORED_TERMINAL 0
#if COLORED_TERMINAL
#define COLOR_RESET "\033[0m"
#define COLOR_RED   "\033[1;31m"
#define COLOR_GREEN "\033[32m"
#else
#define COLOR_RESET ""
#define COLOR_RED   ""
#define COLOR_GREEN ""
#endif

inline std::ostream* mn_output_stream = &std::cout;
inline void mn_print_colored(const char* color, const char* prefix, const char* fmt, ...) {
	*mn_output_stream << color << prefix;

	char buffer[1024];
	va_list args;
	va_start(args, fmt);
	std::vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	*mn_output_stream << buffer << COLOR_RESET;
}
inline void mn_print_new_line() {
	*mn_output_stream << std::endl;
}



#if MN_DEBUG
	#define MN_LOG(fmt, ...) do { mn_print_colored(COLOR_RESET, "[LOG] ", fmt, ##__VA_ARGS__); mn_print_new_line(); } while (0)
	#define MN_ERROR(fmt, ...) do { mn_print_colored(COLOR_RED, "[ERROR] ", fmt, ##__VA_ARGS__); mn_print_new_line(); } while (0)
#else
	#define MN_LOG(fmt, ...)
	#define MN_ERROR(fmt, ...)
#endif

#define MN_PRINT(fmt, ...) mn_print_colored(COLOR_RESET, "", fmt, ##__VA_ARGS__)
#define MN_PRINT_LN(fmt, ...) do { mn_print_colored(COLOR_RESET, "", fmt, ##__VA_ARGS__); mn_print_new_line(); } while (0)
#define MN_PRINT_ERROR(fmt, ...) do { mn_print_colored(COLOR_RED, "", fmt, ##__VA_ARGS__); mn_print_new_line(); } while (0)

