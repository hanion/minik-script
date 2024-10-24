#pragma once

#include "base.h"
#include "expression.h"
#include "minik.h"
#include "statement.h"
#include "token.h"
#include <vector>

namespace minik {

class Parser {
public:
	Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

	std::vector<Ref<Statement>> parse();

private:
	bool is_at_end() const;
	const Token& peek() const;
	const Token& previous() const;
	const Token& advance();
	const Token& consume(TokenType type, std::string message);

	bool match(TokenType type);
	bool check(TokenType type) const;

	Ref<Expression> expression();
	Ref<Expression> equality();
	Ref<Expression> comparison();
	Ref<Expression> term();
	Ref<Expression> factor();
	Ref<Expression> unary();
	Ref<Expression> primary();

	void synchronize();

	Ref<Statement> statement();
	Ref<Statement> print_statement();
	Ref<Statement> expression_statement();

private:
	std::vector<Token> m_tokens;
	int m_current = 0;
};




}
