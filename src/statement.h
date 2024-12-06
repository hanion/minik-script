#pragma once

#include "token.h"
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

struct VariableStatement : public Statement {
	Token name;
	Ref<Expression> initializer;

	VariableStatement(const Token& name, const Ref<Expression>& initializer)
		: name(name), initializer(initializer) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct BlockStatement : public Statement {
	std::vector<Ref<Statement>> statements;
	std::vector<Ref<Statement>> deferred_statements = {};

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

struct ForStatement : public Statement {
	Ref<Statement> initializer;
	Ref<Expression> condition;
	Ref<Expression> increment;
	Ref<BlockStatement> body;

	ForStatement(const Ref<Statement>& initializer, const Ref<Expression>& condition,
			  const Ref<Expression>& increment, const Ref<BlockStatement>& body)
		: initializer(initializer), condition(condition), increment(increment), body(body) {}

	// while
	ForStatement(const Ref<Expression>& condition, const Ref<BlockStatement>& body)
		: initializer(nullptr), condition(condition), increment(nullptr), body(body) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct BreakStatement : public Statement {
	Token keyword;

	BreakStatement(const Token& keyword)
		: keyword(keyword) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};
struct ContinueStatement : public Statement {
	Token keyword;

	ContinueStatement(const Token& keyword)
		: keyword(keyword) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct FunctionStatement : public Statement {
	Token name;
	std::vector<Token> params;
	Ref<BlockStatement> body;

	FunctionStatement(const Token& name, const std::vector<Token>& params, const Ref<BlockStatement>& body)
		: name(name), params(params), body(body) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct ReturnStatement : public Statement {
	Token keyword;
	Ref<Expression> value;

	ReturnStatement(const Token& keyword, const Ref<Expression>& value)
		: keyword(keyword), value(value) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct ClassStatement : public Statement {
	Token name;
	std::vector<Ref<FunctionStatement>> methods;
	std::vector<Ref<VariableStatement>> members;

	ClassStatement(const Token& name, const std::vector<Ref<FunctionStatement>>& methods, const std::vector<Ref<VariableStatement>>& members)
		: name(name), methods(methods), members(members) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct DeferStatement : public Statement {
	Token token;
	Ref<Statement> statement;
	BlockStatement* enclosing_block;

	DeferStatement(const Token& token, const Ref<Statement>& statement, BlockStatement* enclosing_block)
		: token(token), statement(statement), enclosing_block(enclosing_block) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};


}
