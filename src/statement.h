#pragma once

#include "visitor.h"
#include "expression.h"

namespace minik {


struct Statement {
	virtual ~Statement() = default;
	virtual void accept(Visitor& visitor) {}
};

struct ExpressionStatement : public Statement {
	Ref<Expression> expression;

	ExpressionStatement(const Ref<Expression>& expression)
		: expression(expression) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct PrintStatement : public Statement {
	Ref<Expression> expression;

	PrintStatement(const Ref<Expression>& expression)
		: expression(expression) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

}