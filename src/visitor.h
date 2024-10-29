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

class ExpressionStatement;
class PrintStatement;
class VariableStatement;
class BlockStatement;
class IfStatement;
class ForStatement;
class BreakStatement;
class ContinueStatement;
class FunctionStatement;
class ReturnStatement;

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


	virtual void visit(const ExpressionStatement& s) {}
	virtual void visit(const PrintStatement& s) {}
	virtual void visit(const VariableStatement& s) {}
	virtual void visit(const BlockStatement& s) {}
	virtual void visit(const IfStatement& s) {}
	virtual void visit(const ForStatement& s) {}
	virtual void visit(const BreakStatement& s) {}
	virtual void visit(const ContinueStatement& s) {}
	virtual void visit(const FunctionStatement& s) {}
	virtual void visit(const ReturnStatement& s) {}

};

}

