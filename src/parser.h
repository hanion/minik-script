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
	const Token& peek_next() const;
	const Token& previous() const;
	const Token& advance();
	const Token& consume(TokenType type, std::string message);

	bool match(TokenType type);
	bool check(TokenType type) const;
	bool check_next(TokenType type) const;

	Ref<Expression> expression();
	Ref<Expression> assignment();
	Ref<Expression> equality();
	Ref<Expression> comparison();
	Ref<Expression> term();
	Ref<Expression> factor();
	Ref<Expression> unary();
	Ref<Expression> primary();
	Ref<Expression> logical_or();
	Ref<Expression> logical_and();
	Ref<Expression> call();
	Ref<Expression> finish_call(const Ref<Expression>& callee);
	Ref<Expression> array_initializer();

	void synchronize();

	Ref<Statement> statement();
	Ref<Statement> expression_statement();
	Ref<Statement> declaration();
	Ref<Statement> typed_declaration();
	Ref<BlockStatement> block_statement();
	Ref<Statement> if_statement();
	Ref<Statement> while_statement();
	Ref<Statement> for_statement();
	Ref<Statement> break_statement();
	Ref<Statement> continue_statement();
	Ref<FunctionStatement> function(const Token& identifier);
	Ref<Statement> return_statement();
	Ref<Statement> class_declaration(const Token& identifier);
	Ref<Statement> defer_statement();
	Ref<Statement> label_statement();

private:
	std::vector<Token> m_tokens;
	int m_current = 0;
};




}
