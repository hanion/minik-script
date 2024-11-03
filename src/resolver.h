#pragma once
#include "interpreter.h"
#include "visitor.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace minik {

using ResolverScope = std::unordered_map<std::string, bool>;

enum class FunctionType {
	NONE,
	FUNCTION
};

enum class LoopType {
	NONE,
	FOR
};

class Resolver : public Visitor {
public:
	Resolver(Interpreter& interpreter)
		: m_interpreter(interpreter) {}

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
	virtual void visit(const ReturnStatement& s)     override;

	void resolve_block(const std::vector<Ref<Statement>>& statements);
private:
	void resolve(const Ref<Statement>& statement);
	void resolve(const Ref<Expression>& expression);
	void resolve_local(const Expression& expression, const Token& token);
	void resolve_function(const FunctionStatement& s, FunctionType type);


	void begin_scope();
	void end_scope();

	void declare(const Token& name);
	void define(const Token& name);



private:
	Interpreter& m_interpreter;
	std::vector<ResolverScope> m_scopes = {};
	FunctionType m_current_function = FunctionType::NONE;
	LoopType m_current_loop = LoopType::NONE;

};

}
