#pragma once
#include "interpreter.h"
#include "visitor.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace minik {

enum class SymbolState { DECLARED, DEFINED, NAKED_LABEL, LOOP_LABEL };
using ResolverScope = std::unordered_map<std::string, SymbolState>;

enum class FunctionType { NONE, FUNCTION, INITIALIZER, METHOD };
enum class LoopType { NONE, FOR };
enum class ClassType { NONE, CLASS };
enum class NamespaceType { NONE, NAMESPACE };

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

	void resolve_block(const std::vector<Ref<Statement>>& statements);
private:
	void resolve(const Ref<Statement>& statement);
	void resolve(const Ref<Expression>& expression);
	void resolve_local(const Expression& expression, const Token& token);
	void resolve_function(const FunctionStatement& s, FunctionType type);

	bool label_exists(const Token& label, SymbolState state);

	void begin_scope();
	void end_scope();

	void declare(const Token& name);
	void define(const Token& name) { define(name.lexeme); }
	void define(const std::string& name);



private:
	Interpreter& m_interpreter;
	std::vector<ResolverScope> m_scopes = {{}};
	FunctionType m_current_function = FunctionType::NONE;
	LoopType m_current_loop = LoopType::NONE;
	ClassType m_current_class = ClassType::NONE;
	NamespaceType m_current_namespace = NamespaceType::NONE;

	BlockStatement* m_current_block = nullptr;

};

}
