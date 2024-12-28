#pragma once

#include "base.h"
#include "object.h"
#include <string>
#include <variant>

namespace minik {

enum TokenType {
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	LEFT_BRACKET, RIGHT_BRACKET,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
	COLON,

	PLUS_PLUS, MINUS_MINUS, MOD,

	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

	IDENTIFIER, STRING, NUMBER,

	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	NAMESPACE,
	RETURN, SUPER, THIS, TRUE, WHILE,
	BREAK, CONTINUE,
	LABEL, GOTO,
	DEFER,
	MEOF
};

const static std::string token_type_to_string(TokenType type) {
	switch (type) {
		case LEFT_PAREN:      return "LEFT_PAREN";
		case RIGHT_PAREN:     return "RIGHT_PAREN";
		case LEFT_BRACE:      return "LEFT_BRACE";
		case RIGHT_BRACE:     return "RIGHT_BRACE";
		case LEFT_BRACKET:    return "LEFT_BRACKET";
		case RIGHT_BRACKET:   return "RIGHT_BRACKET";
		case COMMA:           return "COMMA";
		case DOT:             return "DOT";
		case MINUS:           return "MINUS";
		case PLUS:            return "PLUS";
		case SEMICOLON:       return "SEMICOLON";
		case SLASH:           return "SLASH";
		case STAR:            return "STAR";

		case COLON:           return "COLON";
		case PLUS_PLUS:       return "PLUS_PLUS";
		case MINUS_MINUS:     return "MINUS_MINUS";
		case MOD:             return "MOD";

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
		case NAMESPACE:       return "NAMESPACE";
		case ELSE:            return "ELSE";
		case FALSE:           return "FALSE";
		case FUN:             return "FUN";
		case FOR:             return "FOR";
		case IF:              return "IF";
		case NIL:             return "NIL";
		case OR:              return "OR";
		case RETURN:          return "RETURN";
		case SUPER:           return "SUPER";
		case THIS:            return "THIS";
		case TRUE:            return "TRUE";
		case WHILE:           return "WHILE";
		case BREAK:           return "BREAK";
		case CONTINUE:        return "CONTINUE";

		case DEFER:           return "DEFER";
		case LABEL:           return "LABEL";
		case GOTO:            return "GOTO";

		case MEOF:            return "MEOF";
	}
	return "UNKNOWN_TOKEN";
}


class Token {
public:
	TokenType type;
	std::string lexeme;
	Ref<Object> literal;
	uint32_t line;

	Token(TokenType type, std::string lexeme, Ref<Object> literal, uint32_t line)
		: type(type), lexeme(lexeme), literal(literal), line(line) {}


	std::string to_string() const {
		return token_type_to_string(type) + " " + lexeme + " " + literal->to_string();
	}

};


static const Token THIS_TOKEN = {IDENTIFIER, "this", {}, 0};
static const Token NAMESPACE_TOKEN = {IDENTIFIER, "namespace", {}, 0};

}
