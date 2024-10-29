#pragma once

#include "callable.h"
#include "function.h"
#include "environment.h"
#include "expression.h"
#include "minik.h"
#include "object.h"
#include "statement.h"
#include <vector>

namespace minik {

class Interpreter : public Visitor {
public:
	Interpreter();

	virtual void visit(const LiteralExpression& e)    override;
	virtual void visit(const BinaryExpression& e)     override;
	virtual void visit(const UnaryExpression& e)      override;
	virtual void visit(const GroupingExpression& e)   override;
	virtual void visit(const VariableExpression& e)   override;
	virtual void visit(const AssignmentExpression& e) override;
	virtual void visit(const LogicalExpression& e)    override;
	virtual void visit(const CallExpression& e)       override;

	virtual void visit(const ExpressionStatement& s) override;
	virtual void visit(const PrintStatement& s)      override;
	virtual void visit(const VariableStatement& s)   override;
	virtual void visit(const BlockStatement& s)      override;
	virtual void visit(const IfStatement& s)         override;
	virtual void visit(const ForStatement& s)        override;
	virtual void visit(const BreakStatement& s)      override;
	virtual void visit(const ContinueStatement& s)   override;
	virtual void visit(const FunctionStatement& s)   override;

	void interpret(const std::vector<Ref<Statement>>& statements);

private:
	Object evaluate(const Ref<Expression>& expression);
	bool is_equal(const Token& token, const Object& a, const Object& b) const;
	bool is_truthy(const Token& token, const Object& object) const;
	bool is_truthy(const Object& object) const;
	void execute(const Ref<Statement>& statement);
	void execute_block(const std::vector<Ref<Statement>>& statements, const Ref<Environment>& environment);


private:
	Ref<Environment> m_globals = CreateRef<Environment>();
	Ref<Environment> m_environment = m_globals;
	Object m_result;
friend MinikFunction;
};

}

