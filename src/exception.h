#pragma once

#include "token.h"
#include <exception>
#include <string>

namespace minik {

class ParseException : public std::exception {
public:
	const std::string msg;
	const Token token;
	ParseException(const Token& token, const std::string& message) : msg(message), token(token) {
		if (token.type == MEOF) {
			report_error(token.line, msg);
		} else {
			report_error(token.line, message + " at: '" + token.lexeme + "'.");
		}
	}
	virtual const char* what() const noexcept override { return msg.c_str(); }
};

class InterpreterException : public std::exception {
public:
	const Token token;
	const Object object;
	const std::string msg;
	InterpreterException(const Token& token, const Object& object, const std::string& message)
		: token(token), object(object), msg(message) {
		report_error(token.line, message + " at: '" + object.to_string() + "'.");
	}
	InterpreterException(const Token& token, const std::string& message)
		: token(token), object(false), msg(message) {
		report_error(token.line, message);
	}

	InterpreterException(const Token& token, const Ref<Object>& object, const std::string& message)
		: token(token), object(*object.get()), msg(message) {
		report_error(token.line, message + " at: '" + object->to_string() + "'.");
	}
	virtual const char* what() const noexcept override { return msg.c_str(); }
};


void report_parse_error(const ParseException& e);
void report_runtime_error(const InterpreterException& e);




struct BreakException {
	const Token label;
};
struct ContinueException {
	const Token label;
};
struct GotoException {
	const Token label;
};
struct AssertException {};

struct ReturnException {
	Ref<Object> value;
};

}
