#include "lexer.h"
#include "minik.h"
#include "token.h"
#include <cstddef>
#include <string>

namespace minik {

const std::map<std::string, TokenType> Lexer::keywords = {
	{"and",    AND},
	{"break",  BREAK},
	{"class",  CLASS},
	{"continue", CONTINUE},
	{"else",   ELSE},
	{"defer",  DEFER},
	{"false",  FALSE},
	{"for",    FOR},
	{"fun",    FUN},
	{"if",     IF},
	{"nil",    NIL},
	{"or",     OR},
	{"print",  PRINT},
	{"return", RETURN},
	{"super",  SUPER},
	{"this",   THIS},
	{"true",   TRUE},
	{"while",  WHILE}
};


std::vector<Token>& Lexer::scan_tokens() {
	while (!is_at_end()) {
		m_start = m_current;
		scan_token();
	}

	m_tokens.emplace_back(MEOF, "", CreateRef<Object>(nullptr), m_line);

	return m_tokens;
}


void Lexer::scan_token() {
	char c = advance();
	switch (c) {
		case '(': add_token(LEFT_PAREN); break;
		case ')': add_token(RIGHT_PAREN); break;
		case '{': add_token(LEFT_BRACE); break;
		case '}': add_token(RIGHT_BRACE); break;
		case '[': add_token(LEFT_BRACKET); break;
		case ']': add_token(RIGHT_BRACKET); break;
		case ',': add_token(COMMA); break;
		case '.': add_token(DOT); break;
		case '-':
			if (match('-')) {
				add_token(MINUS_MINUS);
			} else {
				add_token(MINUS);
			}
			break;
		case '+':
			if (match('+')) {
				add_token(PLUS_PLUS);
			} else {
				add_token(PLUS);
			}
			break;
		case ';': add_token(SEMICOLON); break;
		case ':': add_token(COLON); break;
		case '*': add_token(STAR); break; 
		case '%': add_token(MOD); break; 
		case '!': add_token(match('=') ? BANG_EQUAL : BANG); break;
		case '=': add_token(match('=') ? EQUAL_EQUAL : EQUAL); break;
		case '<': add_token(match('=') ? LESS_EQUAL : LESS); break;
		case '>': add_token(match('=') ? GREATER_EQUAL : GREATER); break;
		case '/':
			if (match('/')) {
				while (peek() != '\n' && !is_at_end()) {
					advance();
				}
			} else {
				add_token(SLASH);
			}
			break;
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace.
			break;
		case '\n': m_line++; break;
		case '"': string(); break;
		default:
			if (is_digit(c)) {
				number();
			} else if (is_alpha(c)) {
				identifier();
			} else {
				report_error(m_line, "Unexpected character.");
			}
			break;
	}
}

bool Lexer::is_at_end() const {
	return m_current >= m_source.length();
}
char Lexer::advance() {
	return m_source.at(m_current++);
}
void Lexer::add_token(TokenType type) {
	add_token(type, CreateRef<Object>(nullptr));
}
void Lexer::add_token(TokenType type, Object literal) {
	std::string text = substr(m_start, m_current);
	m_tokens.emplace_back(type, text, CreateRef<Object>(literal), m_line);
}

bool Lexer::match(char expected) {
	if (is_at_end()) { return false; }
	if (m_source.at(m_current) != expected) { return false; }
	m_current++;
	return true;
}
char Lexer::peek() const {
	if (is_at_end()) { return '\0'; }
	return m_source.at(m_current);
}
char Lexer::peek_next() const {
	if (m_current + 1 >= m_source.length()) { return '\0'; }
	return m_source.at(m_current + 1);
}


void Lexer::string() {
	while (peek() != '"' && !is_at_end()) {
		if (peek() == '\n') {
			m_line++;
		}
		advance();
	}

	if (is_at_end()) {
		report_error(m_line, "Unterminated string.");
		return;
	}

	// closing "
	advance();

	// trim the surrounding quotes
	std::string value = substr(m_start + 1, m_current - 1);
	add_token(STRING, {value});
}


bool Lexer::is_digit(char c) const {
	return c >= '0' && c <= '9';
}

void Lexer::number() {
	while (is_digit(peek())) {
		advance();
	}

	if (peek() == '.' && is_digit(peek_next())) {
		// consume the .
		advance();

		while (is_digit(peek())) {
			advance();
		}
	}

	add_token(NUMBER, { std::stod(substr(m_start, m_current)) });
}


bool Lexer::is_alpha(char c) const {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

void Lexer::identifier() {
	while (is_alpha_numeric(peek())) {
		advance();
	}

	std::string text = substr(m_start, m_current);

	TokenType type = IDENTIFIER;
	if (keywords.find(text) != keywords.end()) {
		type = keywords.at(text);
	}

	add_token(type);
}

bool Lexer::is_alpha_numeric(char c) const {
	return is_alpha(c) || is_digit(c);
}

std::string Lexer::substr(int start, int end) const {
	return m_source.substr(start, (end-start));
}


}
