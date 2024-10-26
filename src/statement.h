#pragma once

#include "visitor.h"
#include "expression.h"
#include <vector>

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

struct VariableStatement : public Statement {
	Token name;
	Ref<Expression> initializer;

	VariableStatement(const Token& name, const Ref<Expression>& initializer)
		: name(name), initializer(initializer) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct BlockStatement : public Statement {
	std::vector<Ref<Statement>> statements;

	BlockStatement(const std::vector<Ref<Statement>>& statements)
		: statements(statements) {}
	BlockStatement(const Ref<Statement>& statement)
		: statements({statement}) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct IfStatement : public Statement {
	Ref<Expression> condition;
	Ref<BlockStatement> then_branch;
	Ref<BlockStatement> else_branch;

	IfStatement(const Ref<Expression>& condition, const Ref<BlockStatement>& then_branch, const Ref<BlockStatement>& else_branch)
		: condition(condition), then_branch(then_branch), else_branch(else_branch) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

}
