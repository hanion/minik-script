#pragma once

#include "token.h"
#include "visitor.h"


namespace minik {

struct Expression {
	virtual ~Expression() = default;
	virtual void accept(Visitor& visitor) {}
};


struct LiteralExpression : public Expression {
	Literal value;

	LiteralExpression(Literal val)
		: value(val) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct BinaryExpression : public Expression {
	Ref<Expression> left;
	Token operator_token;
	Ref<Expression> right;

	BinaryExpression(Ref<Expression> l, Token op, Ref<Expression> r)
		: left(l), operator_token(op), right(r) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct UnaryExpression : public Expression {
	Token operator_token;
	Ref<Expression> right;

	UnaryExpression(Token op, Ref<Expression> rhs)
		: operator_token(op), right(rhs) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct GroupingExpression : public Expression {
	Ref<Expression> expression;

	GroupingExpression(Ref<Expression> expr)
		: expression(expr) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};


}
