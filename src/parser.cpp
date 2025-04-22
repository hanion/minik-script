#include "parser.h"
#include "base.h"
#include "exception.h"
#include "expression.h"
#include "lexer.h"
#include "minik.h"
#include "resolver.h"
#include "statement.h"
#include "token.h"
#include <filesystem>
#include <fstream>

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
		} else if (GetExpression* get_expr = dynamic_cast<GetExpression*>(expr.get())) {
			return CreateRef<SetExpression>(get_expr->object, value, get_expr->name);
		} else if (SubscriptExpression* sbs = dynamic_cast<SubscriptExpression*>(expr.get())) {
			return CreateRef<SetSubscriptExpression>(sbs->object, sbs->key, value, sbs->name);
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
	if (match(FALSE)) { return CreateRef<LiteralExpression>(CreateRef<Object>(false)); }
	if (match(TRUE))  { return CreateRef<LiteralExpression>(CreateRef<Object>(true)); }
	if (match(NIL))   { return CreateRef<LiteralExpression>(CreateRef<Object>(nullptr)); }

	if (match(NUMBER) || match(STRING)) {
		return CreateRef<LiteralExpression>(previous().literal);
	}

	if (match(THIS)) {
		return CreateRef<ThisExpression>(previous());
	}

	if (match(IDENTIFIER)) {
		return CreateRef<VariableExpression>(previous());
	}

	if (match(LEFT_PAREN)) {
		Ref<Expression> expr = expression();
		consume(RIGHT_PAREN, "Expect ')' after expression.");
		return CreateRef<GroupingExpression>(expr);
	}

	if (match(LEFT_BRACE)) {
		return array_initializer();
	}
	if (match(LEFT_BRACKET)) {
		return array_size_initializer();
	}

	throw ParseException(peek(), "Expected expression.");
	return nullptr;
}

Ref<Expression> Parser::array_initializer() {
	std::vector<Ref<Expression>> elements;
	if (!check(RIGHT_BRACE)) {
		do {
			elements.push_back(expression());
		} while (match(COMMA));
	}

	consume(RIGHT_BRACE, "Expect '}' after array elements.");
	return CreateRef<ArrayInitializerExpression>(elements, previous());
}

Ref<Expression> Parser::array_size_initializer() {
	Ref<Expression> size = expression();
	consume(RIGHT_BRACKET, "Expect ']' after array size.");
	return CreateRef<ArrayInitSizeExpression>(size, previous());
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
		} else if (match(DOT)) {
			Token name = consume(IDENTIFIER, "Expected property name after '.'.");
			expr = CreateRef<GetExpression>(expr, name);
		} else if (check(LEFT_BRACKET)) {
			Token name = previous();
			match(LEFT_BRACKET);
			Ref<Expression> key = expression();
			consume(RIGHT_BRACKET, "Expected ']' after subscript expression.");
			expr = CreateRef<SubscriptExpression>(expr, key, name);
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
	if (match(WHILE)) {
		return while_statement();
	}
	if (match(LEFT_BRACE)) {
		return block_statement();
	}
	if (match(DEFER)) {
		return defer_statement();
	}
	if (match(LABEL)) {
		return label_statement();
	}
	if (match(GOTO)) {
		return goto_statement();
	}

	if (match(IMPORT)) {
		return import_statement();
	}

	return expression_statement();
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
		if (match(IDENTIFIER)) {
			// NOTE: this should not be IDENTIFIER, but a type
		}

		// second colon
		if (check(COLON)) {
			// either constant, function, struct, class, enum
			if (check_next(LEFT_PAREN)) {
				match(COLON);
				return function(identifier);
			} else if (check_next(CLASS)) {
				match(COLON);
				match(CLASS);
				return class_declaration(identifier);
			} else if (check_next(NAMESPACE)) {
				match(COLON);
				match(NAMESPACE);
				return namespace_declaration(identifier);
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
		condition = CreateRef<LiteralExpression>(CreateRef<Object>(true));
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
	// break with label
	if (match(IDENTIFIER)) {
		const Token& label = previous();
		consume(SEMICOLON, "Expected ';' aftrer 'break' label.");
		return CreateRef<BreakStatement>(label);
	}

	const Token& token = consume(SEMICOLON, "Expected ';' aftrer 'break'.");
	return CreateRef<BreakStatement>(token);
}
Ref<Statement> Parser::continue_statement() {
	// continue with label
	if (match(IDENTIFIER)) {
		const Token& label = previous();
		consume(SEMICOLON, "Expected ';' aftrer 'continue' label.");
		return CreateRef<ContinueStatement>(label);
	}

	const Token& token = consume(SEMICOLON, "Expected ';' aftrer 'continue'.");
	return CreateRef<ContinueStatement>(token);
}

Ref<FunctionStatement> Parser::function(const Token& identifier) {
	consume(LEFT_PAREN, "Expected '(' at function declaration.");
	std::vector<Token> parameters = {};
	if (!check(RIGHT_PAREN)) {
		do {
			if (parameters.size() >= 255) {
				report_error(peek().line, "Can't have more than 255 arguments.");
			}

			parameters.emplace_back(consume(IDENTIFIER, "Expected parameter name."));
			if (match(COLON)) {
				// TODO: parameter type
				consume(IDENTIFIER, "Expected parameter type after ':'.");
			}
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

Ref<Statement> Parser::class_declaration(const Token& identifier) {
	consume(LEFT_BRACE, "Expected '{' after class declaration.");

	std::vector<Ref<FunctionStatement>> methods = {};
	std::vector<Ref<VariableStatement>> members = {};
	while (!check(RIGHT_BRACE) && !is_at_end()) {
		if (check(IDENTIFIER) && check_next(COLON)) {
			Ref<Statement> s = typed_declaration();
			if (auto vs = std::dynamic_pointer_cast<VariableStatement>(s)) {
				members.push_back(vs);
			} else if (auto fs = std::dynamic_pointer_cast<FunctionStatement>(s)) {
				methods.push_back(fs);
			}
		} else {
			break;
		}
	}

	consume(RIGHT_BRACE, "Expected '}' after class body.");
	return CreateRef<ClassStatement>(identifier, methods, members);
}

Ref<Statement> Parser::namespace_declaration(const Token& identifier) {
	consume(LEFT_BRACE, "Expected '{' after namespace declaration.");

	std::vector<Ref<Statement>> fields = {};
	while (!check(RIGHT_BRACE) && !is_at_end()) {
		if (check(IDENTIFIER) && check_next(COLON)) {
			Ref<Statement> s = typed_declaration();
			if (auto vs = std::dynamic_pointer_cast<VariableStatement>(s)) {
				fields.push_back(vs);
			} else if (auto fs = std::dynamic_pointer_cast<FunctionStatement>(s)) {
				fields.push_back(fs);
			} else if (auto ns = std::dynamic_pointer_cast<NamespaceStatement>(s)) {
				fields.push_back(ns);
			} else if (auto cs = std::dynamic_pointer_cast<ClassStatement>(s)) {
				fields.push_back(cs);
			}
		} else {
			break;
		}
	}

	consume(RIGHT_BRACE, "Expected '}' after namespace body.");
	return CreateRef<NamespaceStatement>(identifier, fields);
}


Ref<Statement> Parser::defer_statement() {
	const Token& token = previous();
	Ref<Statement> s = statement();

	return CreateRef<DeferStatement>(token, s, nullptr);
}

Ref<Statement> Parser::label_statement() {
	const Token& id = consume(IDENTIFIER, "Expected identifier after label.");
	Ref<LabelStatement> statement = CreateRef<LabelStatement>(id, nullptr);

	if (match(FOR)) {
		Ref<Statement> s = for_statement();
		if (Ref<ForStatement> fs = std::dynamic_pointer_cast<ForStatement>(s)) {
			fs->label = statement;
			statement->loop = fs;
		}
	} else if (match(WHILE)) {
		Ref<Statement> s = while_statement();
		if (Ref<ForStatement> fs = std::dynamic_pointer_cast<ForStatement>(s)) {
			fs->label = statement;
			statement->loop = fs;
		}
	} else {
		consume(SEMICOLON, "Expected ';' after label.");
	}
	
	return statement;
}
Ref<Statement> Parser::goto_statement() {
	const Token& id = consume(IDENTIFIER, "Expected identifier after goto.");
	consume(SEMICOLON, "Expected ';' after goto.");
	return CreateRef<GotoStatement>(id);
}

Ref<Statement> Parser::import_statement() {
	std::string package_name;
	bool is_file = false;

	if (match(STRING)) {
		package_name = previous().literal->to_string();
		is_file = true;
	} else {
		const Token& id = consume(IDENTIFIER, "Expected identifier after import.");
		package_name = id.lexeme;
	}
	Token name = previous();

	std::string as = "";
	if (match(AS)) {
		const Token& ast = consume(IDENTIFIER, "Expected identifier after import as.");
		as = ast.lexeme;
	}

	consume(SEMICOLON, "Expected ';' after import.");


	std::vector<Ref<Statement>> statements = {};

	if (is_file) {
		if (!std::filesystem::exists(package_name)) {
			throw ParseException(name, "import failed. File does not exist.");
		}

		std::ifstream file(std::filesystem::canonical(package_name));
		if (!file.is_open()) {
			throw ParseException(name, "import failed at '" + package_name + "'.");
		}

		std::string source = std::string(
			(std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()
		);

		Lexer lexer = Lexer(source);
		std::vector<Token>& tokens = lexer.scan_tokens();
		Parser parser = Parser(tokens);
		statements = parser.parse();
		file.close();
	}
	return CreateRef<ImportStatement>(name, statements, as, is_file);
}


}
