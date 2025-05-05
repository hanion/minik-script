#include "callable.h"
#include "base.h"
#include "exception.h"
#include "interpreter.h"
#include "log.h"
#include "minik.h"
#include "object.h"
#include "token.h"
#include <chrono>
#include <cmath>

namespace minik {

Ref<Object> MinikCallable::call(Interpreter& interpreter, const Arguments& arguments) {
	return CreateRef<Object>(404.404);
}

Ref<Object> mcClock::call(Interpreter& interpreter, const Arguments& arguments) {
	return CreateRef<Object>(std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count());
}

Ref<Object> mcAssert::call(Interpreter& interpreter, const Arguments& arguments) {
	Token error_token = Token{IDENTIFIER, "assert", {}, 0};
	if (arguments.size() == 0) {
		throw InterpreterException(error_token, "assert expects at least one argument.");
	}

	if (!arguments[0]->is_bool()) {
		throw InterpreterException(error_token, "assert expects bool as first argument.");
	}
	bool condition = arguments[0]->as_bool();

	if (arguments.size() == 1) {
		if (!condition) {
			throw AssertException();
		}
	} else if (arguments.size() == 2) {
		if (!condition) {
			if (!arguments[1]->is_string()) {
				throw InterpreterException(error_token, "assert expects string as second argument.");
			}
			MN_PRINT_ERROR(arguments[1]->as_string().c_str());
			throw AssertException();
		}
	}

	return CreateRef<Object>(true);
}

Ref<Object> mcToString::call(Interpreter& interpreter, const Arguments& arguments) {
	const auto& a = arguments[0];
	if (a->is_double()) {
		double d = a->as_double();
		if (d == (int)d) {
			return CreateRef<Object>(std::to_string((int)d));
		}
		return CreateRef<Object>(to_string());
	}
	return CreateRef<Object>(a->to_string());
}

Ref<Object> mcPrint::call(Interpreter& interpreter, const Arguments& arguments) {
	Token error_token = Token{IDENTIFIER, "print", {}, 0};
	if (arguments.size() == 0) {
		throw InterpreterException(error_token, error_token.lexeme + " expects at least one argument.");
	}

	std::string str;
	for (size_t i = 0; i < arguments.size(); ++i) {
		if (arguments[i]) {
			str += arguments[i]->to_string();
		} else {
			str += "[NULL]";
		}
		if (i != arguments.size()-1) {
			str += " ";
		}
	}

	MN_PRINT_LN("%s", str.c_str());

	return CreateRef<Object>(str);
}

}
