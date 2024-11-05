#include "interpreter.h"
#include "base.h"
#include "class.h"
#include "environment.h"
#include "exception.h"
#include "expression.h"
#include "function.h"
#include "log.h"
#include "minik.h"
#include "statement.h"
#include "token.h"
#include "callable.h"
#include <cassert>
#include <string>
#include <chrono>

namespace minik {

Interpreter::Interpreter() {
	m_globals->define(Token(IDENTIFIER, "clock", {}, 0), Object{ CreateRef<mcClock>() });
	m_globals->define(Token(IDENTIFIER, "assert", {}, 0), Object{ CreateRef<mcAssert>() });
}


void Interpreter::interpret(const std::vector<Ref<Statement>>& statements) {
	try {
		for (const Ref<Statement>& statement : statements) {
			execute(statement);
		}
	} catch (BreakException) {
	} catch (ContinueException) {
	} catch (InterpreterException e) {
		report_runtime_error(e);
	}
}

void Interpreter::resolve(const Expression& expression, int depth) {
	m_locals[&expression] = depth;
}

Object Interpreter::look_up_variable(const Token& name, const Expression& expression) {
	auto it = m_locals.find(&expression);
	if (it != m_locals.end()) {
		int distance = it->second;
		return m_environment->get_at(distance, name);
	}
	return m_globals->get(name);
}


void Interpreter::visit(const LiteralExpression& e) {
	m_result = e.value;
}

void Interpreter::visit(const GroupingExpression& e) {
	evaluate(e.expression);
}
void Interpreter::visit(const VariableExpression& e) {
	m_result = look_up_variable(e.name, e);
}
void Interpreter::visit(const AssignmentExpression& e) {
	Object value = evaluate(e.value);

	auto it = m_locals.find(&e);
	if (it != m_locals.end()) {
		int distance = it->second;
		m_environment->get_at(distance, e.name) = value;
	} else {
		m_globals->get(e.name) = value;
	}

	m_result = value;
}
void Interpreter::visit(const LogicalExpression& e) {
	Object left = evaluate(e.left);

	if (e.operator_token.type == OR) {
		if (is_truthy(left)) {
			m_result = left;
			return;
		}
	} else if (e.operator_token.type == AND) {
		if (!is_truthy(left)) {
			m_result = left;
			return;
		}
	}

	m_result = evaluate(e.right);
}

void Interpreter::visit(const CallExpression& e) {
	Object callee = evaluate(e.callee);

	std::vector<Object> arguments;
	arguments.reserve(e.arguments.size());
	for (const Ref<Expression>& argument : e.arguments) {
		arguments.emplace_back(evaluate(argument));
	}

	if (!callee.is_callable()) {
		m_result = {};
		throw InterpreterException(e.paren, "Object is not callable.");
	}

	const Ref<MinikCallable>& function = callee.as_callable();

	if (function->arity() != -1 && arguments.size() != function->arity()) {
		throw InterpreterException(e.paren, "Expected " +
						 std::to_string(function->arity()) + " arguments but got " +
						 std::to_string(arguments.size()) + ".");
	}

	try {
		m_result = function->call(*this, arguments);
	} catch (AssertException) {
		throw InterpreterException(e.paren, "Assertion failed.");
	}
}

void Interpreter::visit(const GetExpression& e) {
	Object object = evaluate(e.object);
	if (object.is_instance()) {
		m_result = object.as_instance()->get(e.name, object.as_instance());
		return;
	}

	throw InterpreterException(e.name, "Attempted to access property of a non-instance object.");
}
void Interpreter::visit(const SetExpression& e) {
	Object object = evaluate(e.object);
	if (!object.is_instance()) {
		throw InterpreterException(e.name, "Attempted to access property of a non-instance object.");
	}

	Object value = evaluate(e.value);
	object.as_instance()->set(e.name, CreateRef<Object>(value));
	m_result = value;
}

void Interpreter::visit(const ThisExpression& e) {
	m_result = look_up_variable(e.keyword, e);
}

void Interpreter::visit(const FunctionStatement& s) {
	Ref<MinikFunction> function = CreateRef<MinikFunction>(s, m_environment);
	m_environment->define(s.name, Object{function});
}

void Interpreter::visit(const ReturnStatement& s) {
	Object value;
	if (s.value) {
		value = evaluate(s.value);
	}
	throw ReturnException{value};
}

void Interpreter::visit(const ClassStatement& s) {
 	m_environment->define(s.name, {});

	MethodsMap methods(s.methods.size());
	for (const Ref<FunctionStatement>& method : s.methods) {
		bool is_initializer = (method->name.lexeme == s.name.lexeme);
		methods[method->name.lexeme] = CreateRef<MinikFunction>(*method.get(), m_environment, is_initializer);
	}

	MembersMap members(s.members.size());
	for (const Ref<VariableStatement>& member : s.members) {
		members[member->name.lexeme] = member;
	}

 	m_environment->get(s.name).value = CreateRef<MinikClass>(s.name.lexeme, methods, members);
}

void Interpreter::visit(const UnaryExpression& e) {
	Object right = evaluate(e.right);

	switch (e.operator_token.type) {
		case BANG:
			m_result.value = !is_truthy(e.operator_token, right);
			return;
		case MINUS:
			if (right.is_double()) {
				m_result.value = -right.as_double();
				return;
			}
			throw InterpreterException(e.operator_token, right, "Invalid argument type to unary expression.");
		case PLUS_PLUS: {
			if (right.is_double()) {
				auto var = dynamic_cast<const VariableExpression*>(e.right.get());
				if (var) {
					const Token& varName = var->name;
					Object& value = m_environment->get(varName);
					value = Object{right.as_double()+1};
					m_result = value;
					return;
				}
				return;
			}
			throw InterpreterException(e.operator_token, right, "Invalid argument type to unary expression.");
		}
		case MINUS_MINUS: {
			if (right.is_double()) {
				auto var = dynamic_cast<const VariableExpression*>(e.right.get());
				if (var) {
					const Token& varName = var->name;
					Object& value = m_environment->get(varName);
					value = Object{right.as_double()-1};
					m_result = value;
					return;
				}
				return;
			}
			throw InterpreterException(e.operator_token, right, "Invalid argument type to unary expression.");
		}
		default:
			MN_ERROR("Unreachable. Interpreter visit unary");
			return;
	}
}

void Interpreter::visit(const BinaryExpression& e) {
	Object left = evaluate(e.left);
	Object right = evaluate(e.right);


	// string concatenation
	if (e.operator_token.type == PLUS && left.is_string() && right.is_string()) {
		m_result.value = left.as_string() + right.as_string();
		return;
	}


	// is equals
	switch (e.operator_token.type) {
		case EQUAL_EQUAL:
			m_result.value = is_equal(e.operator_token, left, right);
			return;
		case BANG_EQUAL:
			m_result.value = !is_equal(e.operator_token, left, right);
			return;
		default:
			break;
	}


	// doubles
	if (!left.is_double()) {
		throw InterpreterException(e.operator_token, left, "Invalid operand to binary expression.");
	}
	if (!right.is_double()) {
		throw InterpreterException(e.operator_token, right, "Invalid operand to binary expression.");
	}
	double l = left.as_double();
	double r = right.as_double();

	switch (e.operator_token.type) {
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
