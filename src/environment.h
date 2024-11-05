#pragma once

#include "exception.h"
#include "log.h"
#include "minik.h"
#include "object.h"
#include <cassert>
#include <string>
#include <unordered_map>

namespace minik {

class Environment {
public:
	Environment() : enclosing(nullptr) {}
	Environment(const Ref<Environment>& enclosing) : enclosing(enclosing) {}


	void define(const Token& name, const Object& value) {
		if (values.count(name.lexeme) > 0) {
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

	Object& get_at(int distance, const Token& name) {
		Environment& env = ancestor(distance);

		auto it = env.values.find(name.lexeme);
		if (it != env.values.end()) {
			return it->second;
		}

		// UNREACHABLE
		MN_ERROR("Environment::get_at Interpreter couldn't find token in environment. [line %d] (distance %d, token '%s')",
		   name.line, distance, name.lexeme.c_str());
		assert(false);
		return *CreateRef<Object>().get();
	}

	Environment& ancestor(int distance) {
		Environment* env = this;
		for (int i = 0; i < distance; ++i) {
			env = env->enclosing.get();
		}
		return *env;
	}


private:
	Ref<Environment> enclosing;
	std::unordered_map<std::string, Object> values = {};
};

}

