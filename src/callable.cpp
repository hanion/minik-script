#include "callable.h"
#include "base.h"
#include "exception.h"
#include "interpreter.h"
#include "log.h"
#include "minik.h"
#include "object.h"
#include "token.h"
#include <chrono>

namespace minik {

Object MinikCallable::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	return Object{404.404};
}

Object mcClock::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	Object result;
	result.value = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
	return result;
}

Object mcAssert::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	Token error_token = Token{IDENTIFIER, "assert", {}, 0};
	if (arguments.size() == 0) {
		throw InterpreterException(error_token, "assert expects at least one argument.");
	}

	if (!arguments[0].is_bool()) {
		throw InterpreterException(error_token, "assert expects bool as first argument.");
	}
	bool condition = arguments[0].as_bool();

	if (arguments.size() == 1) {
		if (!condition) {
			throw AssertException();
		}
	} else if (arguments.size() == 2) {
		if (!condition) {
			if (!arguments[1].is_string()) {
				throw InterpreterException(error_token, "assert expects string as second argument.");
			}
			MN_PRINT_ERROR(arguments[1].as_string().c_str());
			throw AssertException();
		}
	}

	return Object{true};
}

}
