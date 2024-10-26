#include "parser.h"
#include "exception.h"
#include "expression.h"
#include "minik.h"
#include "statement.h"
#include "token.h"

namespace minik {


std::vector<Ref<Statement>> Parser::parse() {
	std::vector<Ref<Statement>> statements;
	
	while (!is_at_end()) {
		try {
			statements.emplace_back(declaration());
		} catch (ParseException e) {
			report_parse_error(e);
			break;
		}
	}

	return statements;
}


bool Parser::is_at_end() const {
	return peek().type == MEOF;
}

const Token& Parser::peek() const {
	return m_tokens[m_current];
}
const Token& Parser::peek_next() const {
	return m_tokens[m_current + 1];
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
bool Parser::check_next(TokenType type) const {
	if (m_current + 1 >= m_tokens.size()) { return false; }
	return peek_next().type == type;
}



Ref<Expression> Parser::expression() {
	return assignment();
}

Ref<Expression> Parser::assignment() {
	Ref<Expression> expr = equality();

	if (match(EQUAL)) {
		Token equals = previous();
		Ref<Expression> value = assignment();

		if (VariableExpression* var_expr = dynamic_cast<VariableExpression*>(expr.get())) {
			Token name = var_expr->name;
			return CreateRef<AssignmentExpression>(name, value);
		 }

		 report_error(equals.line, "Invalid assignment target."); 
	}

	return expr;
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

	if (match(IDENTIFIER)) {
		return CreateRef<VariableExpression>(previous());
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




Ref<Statement> Parser::statement() {
	if (match(PRINT)) {
		return print_statement();
	}
	if (match(LEFT_BRACE)) {
		return CreateRef<BlockStatement>(block());
	}

	return expression_statement();
}

Ref<Statement> Parser::print_statement() {
	Ref<Expression> value = expression();
	consume(SEMICOLON, "Expected ';' after value.");
	return CreateRef<PrintStatement>(value);
}

Ref<Statement> Parser::expression_statement() {
	Ref<Expression> expr = expression();
	consume(SEMICOLON, "Expected ';' after expression.");
	return CreateRef<ExpressionStatement>(expr);
}

Ref<Statement> Parser::declaration() {
	try {
		if (check(IDENTIFIER)) {
			if (check_next(COLON)) {
				return typed_declaration();
			}
			if (check_next(EQUAL)) {
				return statement();
			}
		}
		return statement();
	} catch (const ParseException& e) {
		synchronize();
		return nullptr;
	}
}

Ref<Statement> Parser::typed_declaration() {
	Token identifier = consume(IDENTIFIER, "Expected variable name.");

	if (match(COLON)) {
		// TODO: type
	}

	Ref<Expression> initializer = nullptr;
	if (match(EQUAL) || match(COLON)) {
		initializer = expression();
	}

	consume(SEMICOLON, "Expected ';' after declaration.");
	return CreateRef<VariableStatement>(identifier, initializer);
}

std::vector<Ref<Statement>> Parser::block() {
	std::vector<Ref<Statement>> statements = {};

	while (!check(RIGHT_BRACE) && !is_at_end()) {
		statements.emplace_back(declaration());
	}

	consume(RIGHT_BRACE, "Expected '}' after block.");
	return statements;
}


}
