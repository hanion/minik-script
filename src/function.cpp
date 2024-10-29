#include "function.h"
#include "environment.h"
#include "exception.h"
#include "interpreter.h"

namespace minik {

Object MinikFunction::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	Ref<Environment> env = CreateRef<Environment>(m_closure);
	for (int i = 0; i < m_declaration.params.size(); i++) {
		env->define(m_declaration.params[i], arguments[i]);
	}

	try {
		interpreter.execute_block(m_declaration.body->statements, env);
	} catch (ReturnException e) {
		return e.value;
	}
	// TODO: return values
	return {};
}

int MinikFunction::arity() {
	return m_declaration.params.size();
}

std::string MinikFunction::to_string() const {
	return "<fn " + m_declaration.name.lexeme + ">";
}

}
