#pragma once

namespace minik {

class LiteralExpression;
class BinaryExpression;
class UnaryExpression;
class GroupingExpression;
class VariableExpression;
class AssignmentExpression;

class ExpressionStatement;
class PrintStatement;
class VariableStatement;
class BlockStatement;
class IfStatement;

class Visitor {
public:
	virtual ~Visitor() = default;

	virtual void visit(const LiteralExpression& literal) {}
	virtual void visit(const BinaryExpression& binary) {}
	virtual void visit(const UnaryExpression& unary) {}
	virtual void visit(const GroupingExpression& grouping) {}
	virtual void visit(const VariableExpression& grouping) {}
	virtual void visit(const AssignmentExpression& assign) {}


	virtual void visit(const ExpressionStatement& s) {}
	virtual void visit(const PrintStatement& s) {}
	virtual void visit(const VariableStatement& s) {}
	virtual void visit(const BlockStatement& s) {}
	virtual void visit(const IfStatement& s) {}

};

}

