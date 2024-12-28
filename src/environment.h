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

	void predefine(const Token& name, const Ref<Object>& value) {
		values.emplace(name.lexeme, Symbol{false, value});
	}
	void define(const Token& name, const Ref<Object>& value) {
		if (values.count(name.lexeme) > 0) {
			if (values.at(name.lexeme).defined) {
				throw InterpreterException(name, "Redefinition of '" + name.lexeme + "'.");
			} else {
				values.at(name.lexeme).defined = true;
				return;
			}
		}
		values.emplace(name.lexeme, Symbol{true, value});
	}

	bool has(const std::string& name) {
		auto it = values.find(name);
		if (it != values.end()) {
			return true;
		}
		if (enclosing) {
			return enclosing->has(name);
		}
		return false;
	}

	Ref<Object> get(const Token& name) {
		auto it = values.find(name.lexeme);
		if (it != values.end()) {
			return it->second.object;
		}
	
		if (enclosing) {
			return enclosing->get(name);
		}

		throw InterpreterException(name, "Undefined variable '" + name.lexeme + "'.");
	}

	Ref<Object> get_at(int distance, const Token& name) {
		Environment& env = ancestor(distance);

		auto it = env.values.find(name.lexeme);
		if (it != env.values.end()) {
			return it->second.object;
		}

		// UNREACHABLE
		throw InterpreterException(name,
			"Environment::get_at Interpreter couldn't find token in environment. [line "
				+std::to_string(name.line)+"] (distance "+std::to_string(distance)+", token '"+name.lexeme+"')");
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
	struct Symbol {
		bool defined = false;
		Ref<Object> object;
	};
	std::unordered_map<std::string, Symbol> values = {};
};

}

