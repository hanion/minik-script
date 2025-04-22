#pragma once

#include "token.h"
#include "visitor.h"
#include <vector>


namespace minik {

struct Expression {
	virtual ~Expression() = default;
	virtual void accept(Visitor& visitor) {}
};


struct LiteralExpression : public Expression {
	Ref<Object> value;

	LiteralExpression(Ref<Object> val)
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

struct VariableExpression : public Expression {
	Token name;

	VariableExpression(const Token& name)
		: name(name) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct AssignmentExpression : public Expression {
	Token name;
	Ref<Expression> value;

	AssignmentExpression(const Token& name, Ref<Expression> value)
		: name(name), value(value) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct LogicalExpression : public Expression {
	Ref<Expression> left;
	Token operator_token;
	Ref<Expression> right;

	LogicalExpression(Ref<Expression> l, Token op, Ref<Expression> r)
		: left(l), operator_token(op), right(r) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct CallExpression : public Expression {
	Ref<Expression> callee;
	Token paren;
	std::vector<Ref<Expression>> arguments;

	CallExpression(Ref<Expression> callee, Token paren, std::vector<Ref<Expression>> arguments)
		: callee(callee), paren(paren), arguments(arguments) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct GetExpression : public Expression {
	Ref<Expression> object;
	Token name;

	GetExpression(Ref<Expression> object, Token name)
		: object(object), name(name) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};
struct SetExpression : public Expression {
	Ref<Expression> object;
	Ref<Expression> value;
	Token name;

	SetExpression(Ref<Expression> object, Ref<Expression> value, Token name)
		: object(object), value(value), name(name) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct ThisExpression : public Expression {
	Token keyword;

	ThisExpression(Token keyword)
		: keyword(keyword) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct SubscriptExpression : public Expression {
	Ref<Expression> object;
	Ref<Expression> key;
	Token name;

	SubscriptExpression(const Ref<Expression>& object, const Ref<Expression>& key, const Token& name)
		: object(object), key(key), name(name) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct ArrayInitializerExpression : public Expression {
	std::vector<Ref<Expression>> elements;
	Token paren;

	ArrayInitializerExpression(const std::vector<Ref<Expression>>& elements, Token paren)
		: elements(elements), paren(paren) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};
struct ArrayInitSizeExpression : public Expression {
	Ref<Expression> size;
	Token paren;

	ArrayInitSizeExpression(const Ref<Expression>& size, Token paren)
		: size(size), paren(paren) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};

struct SetSubscriptExpression : public Expression {
	Ref<Expression> object;
	Ref<Expression> index;
	Ref<Expression> value;
	Token name;

	SetSubscriptExpression(Ref<Expression> object, Ref<Expression> index, Ref<Expression> value, Token name)
		: object(object), index(index), value(value), name(name) {}

	void accept(Visitor& visitor) override { visitor.visit(*this); }
};


}
