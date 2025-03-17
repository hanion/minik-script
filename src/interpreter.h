#pragma once

#include "callable.h"
#include "class.h"
#include "function.h"
#include "environment.h"
#include "expression.h"
#include "minik.h"
#include "object.h"
#include "statement.h"
#include <unordered_map>
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
	virtual void visit(const GetExpression& e)        override;
	virtual void visit(const SetExpression& e)        override;
	virtual void visit(const ThisExpression& e)       override;
	virtual void visit(const SubscriptExpression& e)  override;
	virtual void visit(const ArrayInitializerExpression& e) override;
	virtual void visit(const SetSubscriptExpression& e) override;

	virtual void visit(const ExpressionStatement& s) override;
	virtual void visit(const VariableStatement& s)   override;
	virtual void visit(const BlockStatement& s)      override;
	virtual void visit(const IfStatement& s)         override;
	virtual void visit(const ForStatement& s)        override;
	virtual void visit(const BreakStatement& s)      override;
	virtual void visit(const ContinueStatement& s)   override;
	virtual void visit(const FunctionStatement& s)   override;
	virtual void visit(const ReturnStatement& s)     override;
	virtual void visit(const ClassStatement& s)      override;
	virtual void visit(const NamespaceStatement& s)  override;
	virtual void visit(const DeferStatement& s)      override;
	virtual void visit(const LabelStatement& s)      override;
	virtual void visit(const GotoStatement& s)       override;

	void interpret(const std::vector<Ref<Statement>>& statements);

	void resolve(const Expression& expression, int depth);

private:
	Ref<Object> look_up_variable(const Token& name, const Expression& expression);

	Ref<Object> evaluate(const Ref<Expression>& expression);
	bool is_equal(const Token& token, const Ref<Object>& a, const Ref<Object>& b) const;
	bool is_truthy(const Token& token, const Ref<Object>& object) const;
	bool is_truthy(const Ref<Object>& object) const;
	void execute(const Ref<Statement>& statement);
	void execute_block(const std::vector<Ref<Statement>>& statements, const Ref<Environment>& environment, const std::vector<Ref<Statement>>& deferred_statements = {});

	void collect_predefinition(Statement* s);
	void collect_predefinitions(const std::vector<Ref<Statement>>& statements);


	Ref<Object> create_class(const ClassStatement& s);
	Ref<Object> create_namespace(const NamespaceStatement& statement);

private:
	Ref<Environment> m_globals = CreateRef<Environment>();
	Ref<Environment> m_environment = m_globals;
	Ref<MinikNamespace> m_namespace = CreateRef<MinikNamespace>("GLOBAL", nullptr);
	std::unordered_map<const Expression*, int> m_locals = {};
	Ref<Object> m_result = nullptr;
friend MinikFunction;
friend MinikCallable;
friend MinikClass;
};

}

