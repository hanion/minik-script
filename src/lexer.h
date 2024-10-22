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
	bool is_at_end() const;
	char advance();
	void add_token(TokenType type);
	void add_token(TokenType type, Literal literal);

	void scan_token();

	bool match(char expected);
	char peek() const;
	char peek_next() const;

	void string();

	bool is_digit(char c) const;
	void number();

	bool is_alpha(char c) const;
	void identifier();

	bool is_alpha_numeric(char c) const;

	inline std::string substr(int start, int end) const;

private:
	std::string m_source;
	std::vector<Token> m_tokens = {};

	static const std::map<std::string, TokenType> keywords;


	int m_start = 0;
	int m_current = 0;
	int m_line = 1;

};

}
