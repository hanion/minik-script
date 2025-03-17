#include "function.h"
#include "base.h"
#include "class.h"
#include "environment.h"
#include "exception.h"
#include "interpreter.h"
#include "token.h"

namespace minik {


Ref<Object> MinikFunction::call(Interpreter& interpreter, const std::vector<Ref<Object>>& arguments) {
	Ref<Environment> env = CreateRef<Environment>(m_closure);
	if (m_callable) {
		return m_callable->call(interpreter, arguments);
	}

	for (int i = 0; i < m_declaration.params.size(); i++) {
		env->define(m_declaration.params[i], arguments[i]);
	}

	if (m_namespace) {
		env->define(NAMESPACE_TOKEN, m_namespace);
	}

	try {
		interpreter.execute_block(m_declaration.body->statements, env, m_declaration.body->deferred_statements);
	} catch (ReturnException e) {
		if (m_is_initializer) {
			return m_closure->get_at(0,THIS_TOKEN);
		}
		return e.value;
	}

	if (m_is_initializer) {
		return m_closure->get_at(0,THIS_TOKEN);
	}
	return nullptr;
}

int MinikFunction::arity() {
	if (m_callable) {
		return m_callable->arity();
	}
	return m_declaration.params.size();
}

std::string MinikFunction::to_string() const {
	if (m_callable) {
		return m_callable->to_string();
	}
	return "<fn " + m_declaration.name.lexeme + ">";
}

Ref<MinikFunction> MinikFunction::bind(const Ref<MinikInstance>& instance) {
	Ref<Environment> env = CreateRef<Environment>(m_closure);
	env->define(THIS_TOKEN, CreateRef<Object>(instance));

	if (m_callable) {
		return CreateRef<MinikFunction>(m_callable, env, m_is_initializer, m_namespace);
	}
	return CreateRef<MinikFunction>(m_declaration, env, m_is_initializer, m_namespace);
}

}
