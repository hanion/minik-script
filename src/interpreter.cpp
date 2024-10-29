#include "interpreter.h"
#include "base.h"
#include "environment.h"
#include "exception.h"
#include "expression.h"
#include "statement.h"

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
	m_result = literal.value;
}

void Interpreter::visit(const GroupingExpression& grouping) {
	evaluate(grouping.expression);
}
void Interpreter::visit(const VariableExpression& variable) {
	m_result = m_environment->get(variable.name);
}
void Interpreter::visit(const AssignmentExpression& assign) {
	Object value = evaluate(assign.value);
	m_environment->get(assign.name) = value;
	m_result = value;
}
void Interpreter::visit(const LogicalExpression& logical) {
	Object left = evaluate(logical.left);

	if (logical.operator_token.type == OR) {
		if (is_truthy(left)) {
			m_result = left;
			return;
		}
	} else if (logical.operator_token.type == AND) {
		if (!is_truthy(left)) {
			m_result = left;
			return;
		}
	}

	m_result = evaluate(logical.right);
}


void Interpreter::visit(const UnaryExpression& unary) {
	Object right = evaluate(unary.right);

	switch (unary.operator_token.type) {
		case BANG:
			m_result.value = !is_truthy(unary.operator_token, right);
			return;
		case MINUS:
			if (right.is_double()) {
				m_result.value = -right.as_double();
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
		m_result.value = left.as_string() + right.as_string();
		return;
	}


	// is equals
	switch (binary.operator_token.type) {
		case EQUAL_EQUAL:
			m_result.value = is_equal(binary.operator_token, left, right);
			return;
		case BANG_EQUAL:
			m_result.value = !is_equal(binary.operator_token, left, right);
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
			m_result.value = l + r;
			return;
		case MINUS:
			m_result.value = l - r;
			return;
		case STAR:
			m_result.value = l * r;
			return;
		case MOD:
			m_result.value = double(int(l) % int(r));
			return;
		case SLASH:
			m_result.value = l / r;
			return;
		case GREATER:
			m_result.value = l > r;
			return;
		case GREATER_EQUAL:
			m_result.value = l >= r;
			return;
		case LESS:
			m_result.value = l < r;
			return;
		case LESS_EQUAL:
			m_result.value = l <= r;
			return;
		default:
			return;
	}
}

Object Interpreter::evaluate(const Ref<Expression>& expression) {
	expression->accept(*this);
	return m_result;
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
}
bool Interpreter::is_truthy(const Object& object) const {
	if (object.is_nil()) {
		return false;
	}
	if (object.is_bool()) {
		return object.as_bool();
	}
	if (object.is_double()) {
		return object.as_double() != 0.0;
	}
	// TODO: throw exception ?
	return false;
}

bool Interpreter::is_equal(const Token& token, const Object& a, const Object& b) const {
	if (a.is_string() != b.is_string()) {
		throw InterpreterException(token, a.is_string() ? a : b, "Cannot compare a string with a non-string type.");
	}
	return a.equals(b);
}

void Interpreter::visit(const ExpressionStatement& s) {
	evaluate(s.expression);
}

void Interpreter::visit(const PrintStatement& s) {
	Object value = evaluate(s.expression);
	MN_PRINT(value.to_string().c_str());
}

void Interpreter::visit(const VariableStatement& s) {
	Object value;
	if (s.initializer) {
		value = evaluate(s.initializer);
	}

	m_environment->define(s.name, value);
}

void Interpreter::visit(const BlockStatement& s) {
	execute_block(s.statements, CreateRef<Environment>(m_environment));
}

void Interpreter::visit(const IfStatement& s) {
	if (is_truthy(evaluate(s.condition))) {
		execute_block(s.then_branch->statements, CreateRef<Environment>(m_environment));
	} else if (s.else_branch) {
		execute_block(s.else_branch->statements, CreateRef<Environment>(m_environment));
	}
}

void Interpreter::visit(const ForStatement& s) {
	const Ref<Environment>& loop_environment = CreateRef<Environment>(m_environment);
	const Ref<Environment> previous = m_environment;
	try {
		m_environment = loop_environment;

		try {
			if (s.initializer) {
				execute(s.initializer);
			}
			while (is_truthy(evaluate(s.condition))) {
				try {
					execute_block(s.body->statements, CreateRef<Environment>(m_environment));
				} catch (ContinueException) {
					// handles continue by ending execution of the block, jumping to the increment
				}
				if (s.increment) {
					evaluate(s.increment);
				}
			}
		} catch (BreakException) {
			// handles break by stopping execution of the loop
		}

	} catch (...) {
		m_environment = previous;
		throw;
	}
	m_environment = previous;
}

void Interpreter::visit(const BreakStatement& s) {
	throw BreakException();
}
void Interpreter::visit(const ContinueStatement& s) {
	throw ContinueException();
}

void Interpreter::execute(const Ref<Statement>& statement) {
	statement->accept(*this);
}

void Interpreter::execute_block(const std::vector<Ref<Statement>>& statements, const Ref<Environment>& environment) {
	const Ref<Environment> previous = m_environment;
	try {
		m_environment = environment;
		for (auto& statement : statements) {
			execute(statement);
		}
	} catch (...) {
		m_environment = previous;
		throw;
	}
	m_environment = previous;
}


}
