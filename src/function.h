#pragma once

#include "callable.h"
#include "environment.h"
#include "statement.h"

namespace minik {

class MinikFunction : public MinikCallable {
public:
	MinikFunction(const FunctionStatement& declaration, const Ref<Environment>& closure, bool is_initializer = false, const Ref<Object>& ns = nullptr)
		: m_declaration(declaration), m_closure(closure), m_is_initializer(is_initializer), m_namespace(ns) {}

	virtual int arity() override;
	virtual std::string to_string() const override;
	virtual Ref<Object> call(Interpreter& interpreter, const std::vector<Ref<Object>>& arguments) override;

	Ref<MinikFunction> bind(const Ref<MinikInstance>& instance);

private:
	FunctionStatement m_declaration;
	Ref<Environment> m_closure;
	Ref<Object> m_namespace;
	bool m_is_initializer;
};

}
