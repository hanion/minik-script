#pragma once

#include "base.h"
#include "expression.h"
#include "minik.h"
#include "token.h"
#include <vector>

namespace minik {

class Parser {
public:
	Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

	Ref<Expression> parse();

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

private:
	std::vector<Token> m_tokens;
	int m_current = 0;
};


class ParseException : public std::exception {
public:
	const std::string msg;
	ParseException(const Token& token, const std::string& message) : msg(message) {
		if (token.type == MEOF) {
			report_error(token.line, msg);
		} else {
			report_error(token.line, "at '" + token.lexeme + "' " + message);
		}
	}
	virtual const char* what() { return msg.c_str(); }
};


}
