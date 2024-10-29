#pragma once

#include "callable.h"
#include "statement.h"

namespace minik {

class MinikFunction : public MinikCallable {
public:
	MinikFunction(const FunctionStatement& declaration) : m_declaration(declaration) {}

	virtual int arity() override;
	virtual std::string to_string() override;
	virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;

private:
	FunctionStatement m_declaration;
};

}
