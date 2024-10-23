#include "parser.h"

namespace minik {


Ref<Expression> Parser::parse() {
	try {
		return expression();
	} catch (ParseException e) {
		return nullptr;
	}
	return nullptr;
}


bool Parser::is_at_end() const {
	return peek().type == MEOF;
}

const Token& Parser::peek() const {
	return m_tokens[m_current];
}

const Token& Parser::previous() const {
	return m_tokens[m_current - 1];
}

const Token& Parser::advance() {
	if (!is_at_end()) {
		m_current++;
	}
	return previous();
}

const Token& Parser::consume(TokenType type, std::string message) {
	if (check(type)) {
		return advance();
	}
	throw ParseException(peek(), message);
	return advance();
}

bool Parser::match(TokenType type) {
	if (check(type)) {
		advance();
		return true;
	}
	return false;
}

bool Parser::check(TokenType type) const {
	if (is_at_end()) { return false; }
	return peek().type == type;
}



Ref<Expression> Parser::expression() {
	return equality();
}

Ref<Expression> Parser::equality() {
	Ref<Expression> expression = comparison();

	while(match(BANG_EQUAL) || match(EQUAL_EQUAL)) {
		const Token& op = previous();
		Ref<Expression> right = comparison();
		expression = CreateRef<BinaryExpression>(expression, op, right);
	}

	return expression;
}

Ref<Expression> Parser::comparison() {
	Ref<Expression> expression = term();

	while(match(GREATER) || match(GREATER_EQUAL)|| match(LESS)|| match(LESS_EQUAL)) {
		const Token& op = previous();
		Ref<Expression> right = term();
		expression = CreateRef<BinaryExpression>(expression, op, right);
	}

	return expression;
}

Ref<Expression> Parser::term() {
	Ref<Expression> expression = factor();

	while(match(MINUS) || match(PLUS)) {
		const Token& op = previous();
		Ref<Expression> right = factor();
		expression = CreateRef<BinaryExpression>(expression, op, right);
	}

	return expression;
}

Ref<Expression> Parser::factor() {
	Ref<Expression> expression = unary();

	while(match(SLASH) || match(STAR)) {
		const Token& op = previous();
		Ref<Expression> right = unary();
		expression = CreateRef<BinaryExpression>(expression, op, right);
	}

	return expression;
}

Ref<Expression> Parser::unary() {
	if (match(BANG) || match(MINUS)) {
		const Token& op = previous();
		Ref<Expression> right = unary();
		return CreateRef<UnaryExpression>(op, right);
	}
	return primary();
}

Ref<Expression> Parser::primary() {
	if (match(FALSE)) { return CreateRef<LiteralExpression>(Literal{false}); }
	if (match(TRUE))  { return CreateRef<LiteralExpression>(Literal{true}); }
	if (match(NIL))   { return CreateRef<LiteralExpression>(Literal{nullptr}); }

	if (match(NUMBER) || match(STRING)) {
		return CreateRef<LiteralExpression>(previous().literal);
	}

	if (match(LEFT_PAREN)) {
		Ref<Expression> expr = expression();
		consume(RIGHT_PAREN, "Expect ')' after expression.");
		return CreateRef<GroupingExpression>(expr);
	}

	throw ParseException(peek(), "Expected expression.");
	return nullptr;
}

void Parser::synchronize() {
	advance();

	while (!is_at_end()) {
		if (previous().type == SEMICOLON) {
			return;
		}

		switch (peek().type) {
			case CLASS:
			case FUN:
			case VAR:
			case FOR:
			case IF:
			case WHILE:
			case PRINT:
			case RETURN:
			return;
			default:
			break;
		}

		advance();
	}
}

}
