#include "minik.h"
#include "exception.h"
#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "ast_printer.cpp"
#include "resolver.h"
#include "statement.h"

#include <cstdlib>
#include <fstream>
#include <string>

namespace minik {

static bool had_error = false;
static bool had_runtime_error = false;
Interpreter interpreter;


void run(const std::string& source) {
	Lexer lexer = Lexer(source);
	std::vector<Token>& tokens = lexer.scan_tokens();
	Parser parser = Parser(tokens);
	std::vector<Ref<Statement>> statements = parser.parse();

	if (had_error) {
		return;
	}

	Resolver resolver = Resolver(interpreter);
	resolver.resolve_block(statements);

	if (had_error) {
		return;
	}

	interpreter.interpret(statements);
}

void run_file(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		MN_ERROR("Could not open file %s", filename.c_str());
		return;
	}

	interpreter = Interpreter();

	std::string source = std::string(
		(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()
	);
	run(source);

	had_error = false;
	had_runtime_error = false;

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
	MN_ERROR("[line %d], %s", line, message.c_str());
}

void report_parse_error(const ParseException& e) {
	had_error = true;
}

void report_runtime_error(const InterpreterException& e) {
	had_runtime_error = true;
}

}
