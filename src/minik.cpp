#include "minik.h"
#include "lexer.h"

#include <cstdlib>
#include <fstream>
#include <string>

namespace minik {

static bool had_error = false;


void run(const std::string& source) {
	MN_LOG("run: %s", source.c_str());
	Lexer l = Lexer(source);
	l.scan_tokens();
}

void run_file(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		MN_ERROR("Could not open file %s", filename.c_str());
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		run(line);

		if (had_error) {
			exit(65);
		}
	}

	file.close();
}


void run_prompt() {
	std::string input;
	std::cout << "minik-script > ";

	while (std::getline(std::cin, input)) {
		if (input.empty()) {
			return;
		}

		run(input);

		had_error = false;

		std::cout << "minik-script > ";
	}
}

void report_error(int line, const std::string& message) {
	had_error = true;
	MN_ERROR("! %d, %s ", line, message.c_str());
}

}
