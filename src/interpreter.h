#pragma once

#include "environment.h"
#include "expression.h"
#include "minik.h"
#include "object.h"
#include "statement.h"
#include <vector>

namespace minik {

class Interpreter : public Visitor {
public:
	virtual void visit(const LiteralExpression& literal)   override;
	virtual void visit(const BinaryExpression& binary)     override;
	virtual void visit(const UnaryExpression& unary)       override;
	virtual void visit(const GroupingExpression& grouping) override;
	virtual void visit(const VariableExpression& variable) override;
	virtual void visit(const AssignmentExpression& assign) override;

	virtual void visit(const ExpressionStatement& s) override;
	virtual void visit(const PrintStatement& s)      override;
	virtual void visit(const VariableStatement& s)   override;
	virtual void visit(const BlockStatement& s)      override;
	virtual void visit(const IfStatement& s)         override;

	void interpret(const std::vector<Ref<Statement>>& statements);

private:
	Object visit(const Ref<Expression>& expression) {
		expression->accept(*this);
		return m_result;
	}

	Object evaluate(const Ref<Expression>& expression);
	bool is_equal(const Token& token, const Object& a, const Object& b) const;
	bool is_truthy(const Token& token, const Object& object) const;
	bool is_truthy(const Object& object) const;
	void execute(const Ref<Statement>& statement);
	void execute_block(const std::vector<Ref<Statement>>& statements, const Ref<Environment>& environment);


private:
	Ref<Environment> m_environment = CreateRef<Environment>();
	Object m_result;
};

}

