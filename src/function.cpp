#include "function.h"
#include "environment.h"
#include "interpreter.h"

namespace minik {

Object MinikFunction::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	Ref<Environment> env = CreateRef<Environment>(interpreter.m_globals);
	for (int i = 0; i < m_declaration.params.size(); i++) {
		env->define(m_declaration.params[i], arguments[i]);
	}

	interpreter.execute_block(m_declaration.body->statements, env);
	// TODO: return values
	return {};
}

int MinikFunction::arity() {
	return m_declaration.params.size();
}

std::string MinikFunction::to_string() {
	return "<fn " + m_declaration.name.lexeme + ">";
}

}
