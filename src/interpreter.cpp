#include "interpreter.h"
#include "exception.h"

namespace minik {

void Interpreter::interpret(const std::vector<Ref<Statement>>& statements) {
	try {
		for (const Ref<Statement>& statement : statements) {
			execute(statement);
		}
	} catch (InterpreterException e) {
		report_runtime_error(e);
	}
}


void Interpreter::visit(const LiteralExpression& literal) {
	result = literal.value;
}

void Interpreter::visit(const GroupingExpression& grouping) {
	evaluate(grouping.expression);
}

void Interpreter::visit(const UnaryExpression& unary) {
	Object right = evaluate(unary.right);

	switch (unary.operator_token.type) {
		case BANG:
			result.value = !is_truthy(unary.operator_token, right);
			return;
		case MINUS:
			if (right.is_double()) {
				result.value = -right.as_double();
				return;
			}
			throw InterpreterException(unary.operator_token, right, "Invalid argument type to unary expression.");
		default:
			MN_ERROR("Unreachable. Interpreter visit unary");
			return;
	}
}

void Interpreter::visit(const BinaryExpression& binary) {
	Object left = evaluate(binary.left);
	Object right = evaluate(binary.right);


	// string concatenation
	if (binary.operator_token.type == PLUS && left.is_string() && right.is_string()) {
		result.value = left.as_string() + right.as_string();
		return;
	}


	// is equals
	switch (binary.operator_token.type) {
		case EQUAL_EQUAL:
			result.value = is_equal(left, right);
			return;
		case BANG_EQUAL:
			result.value = !is_equal(left, right);
			return;
		default:
			break;
	}


	// doubles
	if (!left.is_double()) {
		throw InterpreterException(binary.operator_token, left, "Invalid operand to binary expression.");
	}
	if (!right.is_double()) {
		throw InterpreterException(binary.operator_token, right, "Invalid operand to binary expression.");
	}
	double l = left.as_double();
	double r = right.as_double();

	switch (binary.operator_token.type) {
		case PLUS:
			result.value = l + r;
			return;
		case MINUS:
			result.value = l - r;
			return;
		case STAR:
			result.value = l * r;
			return;
		case SLASH:
			result.value = l / r;
			return;
		case GREATER:
			result.value = l > r;
			return;
		case GREATER_EQUAL:
			result.value = l >= r;
			return;
		case LESS:
			result.value = l < r;
			return;
		case LESS_EQUAL:
			result.value = l <= r;
			return;
		default:
			return;
	}
}

Object Interpreter::evaluate(const Ref<Expression>& expression) {
	expression->accept(*this);
	return result;
}

bool Interpreter::is_truthy(const Token& token, const Object& object) const {
	if (object.is_nil()) {
		return false;
	}
	if (object.is_bool()) {
		return object.as_bool();
	}
	if (object.is_double()) {
		return object.as_double() != 0.0;
	}

	throw InterpreterException(token, object, "No viable conversion to bool.");
	return false;
}

bool Interpreter::is_equal(const Object& a, const Object& b) const {
	if (a.is_nil() && b.is_nil()) {
		return true;
	}
	if (a.is_nil()) {
		return false;
	}
	return a.equals(b);
}

void Interpreter::visit(const ExpressionStatement& s) {
	evaluate(s.expression);
}

void Interpreter::visit(const PrintStatement& s) {
	Object value = evaluate(s.expression);
	MN_LOG(value.to_string().c_str());
}

void Interpreter::execute(const Ref<Statement>& statement) {
	statement->accept(*this);
}

}
