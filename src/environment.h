#pragma once

#include "exception.h"
#include "object.h"
#include <string>
#include <unordered_map>

namespace minik {

class Environment {
public:
	Environment() : enclosing(nullptr) {}
	Environment(const Ref<Environment>& enclosing) : enclosing(enclosing) {}

	bool exists(const Token& name) {
		return values.count(name.lexeme) > 0 || (enclosing && enclosing->exists(name));
	}

	void define(const Token& name, const Object& value) {
		if (exists(name)) {
			throw InterpreterException(name, "Redefinition of '" + name.lexeme + "'.");
		}
		values.emplace(name.lexeme, value);
	}

	Object& get(const Token& name) {
		auto it = values.find(name.lexeme);
		if (it != values.end()) {
			return it->second;
		}
	
		if (enclosing) {
			return enclosing->get(name);
		}

		throw InterpreterException(name, "Undefined variable '" + name.lexeme + "'.");
	}
private:
	Ref<Environment> enclosing;
	std::unordered_map<std::string, Object> values = {};
};

}

