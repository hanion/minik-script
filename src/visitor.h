#pragma once

namespace minik {

class LiteralExpression;
class BinaryExpression;
class UnaryExpression;
class GroupingExpression;

class ExpressionStatement;
class PrintStatement;

class Visitor {
public:
	virtual ~Visitor() = default;

	virtual void visit(const LiteralExpression& literal) {}
	virtual void visit(const BinaryExpression& binary) {}
	virtual void visit(const UnaryExpression& unary) {}
	virtual void visit(const GroupingExpression& grouping) {}

	virtual void visit(const ExpressionStatement& s) {}
	virtual void visit(const PrintStatement& s) {}

};

}

