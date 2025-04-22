#pragma once

namespace minik {

class LiteralExpression;
class BinaryExpression;
class UnaryExpression;
class GroupingExpression;
class VariableExpression;
class AssignmentExpression;
class LogicalExpression;
class CallExpression;
class GetExpression;
class SetExpression;
class ThisExpression;
class SubscriptExpression;
class ArrayInitializerExpression;
class ArrayInitSizeExpression;
class SetSubscriptExpression;

class ExpressionStatement;
class VariableStatement;
class BlockStatement;
class IfStatement;
class ForStatement;
class BreakStatement;
class ContinueStatement;
class FunctionStatement;
class ReturnStatement;
class ClassStatement;
class NamespaceStatement;
class DeferStatement;
class LabelStatement;
class GotoStatement;
class ImportStatement;

class Visitor {
public:
	virtual ~Visitor() = default;

	virtual void visit(const LiteralExpression& e) {}
	virtual void visit(const BinaryExpression& e) {}
	virtual void visit(const UnaryExpression& e) {}
	virtual void visit(const GroupingExpression& e) {}
	virtual void visit(const VariableExpression& e) {}
	virtual void visit(const AssignmentExpression& e) {}
	virtual void visit(const LogicalExpression& e) {}
	virtual void visit(const CallExpression& e) {}
	virtual void visit(const GetExpression& e) {}
	virtual void visit(const SetExpression& e) {}
	virtual void visit(const ThisExpression& e) {}
	virtual void visit(const SubscriptExpression& e) {}
	virtual void visit(const ArrayInitializerExpression& e) {}
	virtual void visit(const ArrayInitSizeExpression& e) {}
	virtual void visit(const SetSubscriptExpression& e) {}


	virtual void visit(const ExpressionStatement& s) {}
	virtual void visit(const VariableStatement& s) {}
	virtual void visit(const BlockStatement& s) {}
	virtual void visit(const IfStatement& s) {}
	virtual void visit(const ForStatement& s) {}
	virtual void visit(const BreakStatement& s) {}
	virtual void visit(const ContinueStatement& s) {}
	virtual void visit(const FunctionStatement& s) {}
	virtual void visit(const ReturnStatement& s) {}
	virtual void visit(const ClassStatement& s) {}
	virtual void visit(const NamespaceStatement& s) {}
	virtual void visit(const DeferStatement& s) {}
	virtual void visit(const LabelStatement& s) {}
	virtual void visit(const GotoStatement& s) {}
	virtual void visit(const ImportStatement& s) {}

};

}

