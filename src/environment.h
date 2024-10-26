#pragma once

#include "exception.h"
#include "object.h"
#include <map>
#include <string>

namespace minik {

class Environment {
public:
	std::map<std::string, Object> values = {};

	void define(const Token& name, const Object& value) {
		if (values.find(name.lexeme) != values.end()) {
			throw InterpreterException(name, "Redefinition of '" + name.lexeme + "'.");
		}
		values.emplace(name.lexeme, value);
	}

	Object& get(const Token& name) {
		auto it = values.find(name.lexeme);
		if (it != values.end()) {
			return it->second;
		}
		throw InterpreterException(name, "Undefined variable '" + name.lexeme + "'.");
	}
};

}

