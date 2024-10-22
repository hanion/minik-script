#pragma once

#include "base.h"
#include <string>
#include <variant>

namespace minik {

enum TokenType {
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

	IDENTIFIER, STRING, NUMBER,

	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
	MEOF
};

const static std::string token_type_to_string(TokenType type) {
	switch (type) {
		case LEFT_PAREN:      return "LEFT_PAREN";
		case RIGHT_PAREN:     return "RIGHT_PAREN";
		case LEFT_BRACE:      return "LEFT_BRACE";
		case RIGHT_BRACE:     return "RIGHT_BRACE";
		case COMMA:           return "COMMA";
		case DOT:             return "DOT";
		case MINUS:           return "MINUS";
		case PLUS:            return "PLUS";
		case SEMICOLON:       return "SEMICOLON";
		case SLASH:           return "SLASH";
		case STAR:            return "STAR";

		case BANG:            return "BANG";
		case BANG_EQUAL:      return "BANG_EQUAL";
		case EQUAL:           return "EQUAL";
		case EQUAL_EQUAL:     return "EQUAL_EQUAL";
		case GREATER:         return "GREATER";
		case GREATER_EQUAL:   return "GREATER_EQUAL";
		case LESS:            return "LESS";
		case LESS_EQUAL:      return "LESS_EQUAL";

		case IDENTIFIER:      return "IDENTIFIER";
		case STRING:          return "STRING";
		case NUMBER:          return "NUMBER";

		case AND:             return "AND";
		case CLASS:           return "CLASS";
		case ELSE:            return "ELSE";
		case FALSE:           return "FALSE";
		case FUN:             return "FUN";
		case FOR:             return "FOR";
		case IF:              return "IF";
		case NIL:             return "NIL";
		case OR:              return "OR";
		case PRINT:           return "PRINT";
		case RETURN:          return "RETURN";
		case SUPER:           return "SUPER";
		case THIS:            return "THIS";
		case TRUE:            return "TRUE";
		case VAR:             return "VAR";
		case WHILE:           return "WHILE";

		case MEOF:            return "MEOF";
		default:              return "UNKNOWN_TOKEN";
	}
}


struct Literal {
	std::variant<double, std::string> value;

	bool is_double() const { return std::holds_alternative<double>(value); }
	bool is_string() const { return std::holds_alternative<std::string>(value); }

	bool is_empty() const {
		if (is_double()) {
			return (std::get<double>(value) == 0);
		}
		if (is_string()) {
			return (std::get<std::string>(value).empty());
		}
		return true;
	}

	std::string to_string() const {
		if (is_empty()) {
			return "";
		}
		if (is_double()) {
			return std::to_string(std::get<double>(value));
		}
		if (is_string()) {
			return std::get<std::string>(value);
		}
		return "";
	}
};


class Token {
public:
	TokenType type;
	std::string lexeme;
	Literal literal;
	uint32_t line;

	Token(TokenType type, std::string lexeme, Literal literal, uint32_t line)
		: type(type), lexeme(lexeme), literal(literal), line(line) {}


	std::string to_string() const {
		return token_type_to_string(type) + " " + lexeme + " " + literal.to_string();
	}

};


}
