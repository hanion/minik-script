#pragma once

#include "callable.h"
#include "environment.h"
#include "statement.h"

namespace minik {

class MinikFunction : public MinikCallable {
public:
	MinikFunction(const FunctionStatement& declaration, const Ref<Environment>& closure)
		: m_declaration(declaration), m_closure(closure) {}

	virtual int arity() override;
	virtual std::string to_string() const override;
	virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;

private:
	FunctionStatement m_declaration;
	Ref<Environment> m_closure;
};

}
