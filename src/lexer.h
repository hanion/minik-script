#pragma once
#include "base.h"
#include "token.h"
#include <vector>
#include <map>


namespace minik {


class Lexer {
public:
	Lexer(std::string source)
		: m_source(source) {}

	std::vector<Token>& scan_tokens();

private:
	bool is_at_end();
	char advance();
	void add_token(TokenType type);
	void add_token(TokenType type, Literal literal);

	void scan_token();

	bool match(char expected);
	char peek();
	char peek_next();

	void string();

	bool is_digit(char c);
	void number();

	bool is_alpha(char c);
	void identifier();

	bool is_alpha_numeric(char c) { return is_alpha(c) || is_digit(c); }

	inline std::string substr(int start, int end);

private:
	std::string m_source;
	std::vector<Token> m_tokens = {};

	static const std::map<std::string, TokenType> keywords;


	int m_start = 0;
	int m_current = 0;
	int m_line = 1;

};

}
