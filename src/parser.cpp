#include "parser.h"
#include "base.h"
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
	Ref<Expression> expr = logical_or();

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

	while(match(MOD) || match(SLASH) || match(STAR)) {
		const Token& op = previous();
		Ref<Expression> right = unary();
		expression = CreateRef<BinaryExpression>(expression, op, right);
	}

	return expression;
}

Ref<Expression> Parser::unary() {
	if (match(BANG) || match(MINUS) || match(PLUS_PLUS) || match(MINUS_MINUS)) {
		const Token& op = previous();
		Ref<Expression> right = unary();
		return CreateRef<UnaryExpression>(op, right);
	}
	return call();
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

Ref<Expression> Parser::logical_or() {
	Ref<Expression> expr = logical_and();

	while (match(OR)) {
		const Token& op = previous();
		Ref<Expression> right = logical_and();
		expr = CreateRef<LogicalExpression>(expr, op, right);
	}

	return expr;
}

Ref<Expression> Parser::logical_and() {
	Ref<Expression> expr = equality();

	while (match(AND)) {
		const Token& op = previous();
		Ref<Expression> right = equality();
		expr = CreateRef<LogicalExpression>(expr, op, right);
	}

	return expr;
}

Ref<Expression> Parser::call() {
	Ref<Expression> expr = primary();
	while (true) {
		if (match(LEFT_PAREN)) {
			expr = finish_call(expr);
		} else {
			break;
		}
	}
	return expr;
}

Ref<Expression> Parser::finish_call(const Ref<Expression>& callee) {
	std::vector<Ref<Expression>> arguments = {};
	if (!check(RIGHT_PAREN)) {
		do {
			if (arguments.size() >= 255) {
				report_error(peek().line, "Can't have more than 255 arguments.");
			}
			arguments.emplace_back(expression());
		} while (match(COMMA));
	}

	Token paren = consume(RIGHT_PAREN, "Expected ')' after arguments.");

	return CreateRef<CallExpression>(callee, paren, arguments);
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
	if (match(RETURN)) {
		return return_statement();
	}
	if (match(BREAK)) {
		return break_statement();
	}
	if (match(CONTINUE)) {
		return continue_statement();
	}
	if (match(FOR)) {
		return for_statement();
	}
	if (match(IF)) {
		return if_statement();
	}
	if (match(PRINT)) {
		return print_statement();
	}
	if (match(WHILE)) {
		return while_statement();
	}
	if (match(LEFT_BRACE)) {
		return block_statement();
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

		// second colon
		if (check(COLON)) {
			// either constant, function, struct, class, enum
			if (check_next(LEFT_PAREN)) {
				match(COLON);
				return function(identifier);
			}
		}
	}

	Ref<Expression> initializer = nullptr;
	if (match(EQUAL) || match(COLON)) {
		initializer = expression();
	}

	consume(SEMICOLON, "Expected ';' after declaration.");
	return CreateRef<VariableStatement>(identifier, initializer);
}

Ref<BlockStatement> Parser::block_statement() {
	std::vector<Ref<Statement>> statements = {};

	while (!check(RIGHT_BRACE) && !is_at_end()) {
		statements.emplace_back(declaration());
	}

	consume(RIGHT_BRACE, "Expected '}' after block.");

	return CreateRef<BlockStatement>(statements);
}

Ref<Statement> Parser::if_statement() {
	Ref<Expression> condition = expression();

	consume(LEFT_BRACE, "Expected '{' after 'if'.");
	Ref<BlockStatement> then_branch = block_statement();
	Ref<BlockStatement> else_branch = nullptr;

	if (match(ELSE)) {
		if (match(IF)) {
			else_branch = CreateRef<BlockStatement>(if_statement());
		} else {
			consume(LEFT_BRACE, "Expected '{' after 'else'.");
			else_branch = block_statement();
		}
	}

	return CreateRef<IfStatement>(condition, then_branch, else_branch);
}

Ref<Statement> Parser::while_statement() {
	Ref<Expression> condition = expression();

	consume(LEFT_BRACE, "Expected '{' after 'while'.");
	Ref<BlockStatement> body = block_statement();

	return CreateRef<ForStatement>(condition, body);
}

Ref<Statement> Parser::for_statement() {
	Ref<Statement> initializer = nullptr;
	if (match(SEMICOLON)) {
		initializer = nullptr;
	} else {
		initializer = declaration();
	}

	Ref<Expression> condition = nullptr;
	if (!check(SEMICOLON)) {
		condition = expression();
	}
	if (condition == nullptr) {
		condition = CreateRef<LiteralExpression>(Literal{true});
	}

	consume(SEMICOLON, "Expected ';' after loop condition.");

	Ref<Expression> increment = nullptr;
	if (!check(LEFT_BRACE)) {
		increment = expression();
	}
	consume(LEFT_BRACE, "Expected '{' after for clauses.");

	Ref<BlockStatement> body = block_statement();

	return CreateRef<ForStatement>(initializer, condition, increment, body);
}

Ref<Statement> Parser::break_statement() {
	consume(SEMICOLON, "Expected ';' aftrer 'break'.");
	return CreateRef<BreakStatement>();
}
Ref<Statement> Parser::continue_statement() {
	consume(SEMICOLON, "Expected ';' aftrer 'continue'.");
	return CreateRef<ContinueStatement>();
}

Ref<Statement> Parser::function(const Token& identifier) {
	consume(LEFT_PAREN, "Expected '(' at function declaration.");
	std::vector<Token> parameters = {};
	if (!check(RIGHT_PAREN)) {
		do {
			if (parameters.size() >= 255) {
				report_error(peek().line, "Can't have more than 255 arguments.");
			}

			parameters.emplace_back(consume(IDENTIFIER, "Expected parameter name."));
		} while (match(COMMA));
	}
	consume(RIGHT_PAREN, "Expected ')' after paramaters.");
	consume(LEFT_BRACE, "Expected '{' after function declaration.");
	Ref<BlockStatement> body = block_statement();
	return CreateRef<FunctionStatement>(identifier, parameters, body);
}


Ref<Statement> Parser::return_statement() {
	Token keyword = previous();
	Ref<Expression> value = nullptr;
	if (!check(SEMICOLON)) {
		value = expression();
	}
	consume(SEMICOLON, "Expected ';' after return value.");
	return CreateRef<ReturnStatement>(keyword, value);
}

}
