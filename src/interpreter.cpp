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
#include <cstddef>
#include <string>
#include <chrono>

namespace minik {

Interpreter::Interpreter() {
	m_globals->define(Token(IDENTIFIER, "clock", {}, 0), CreateRef<Object>( CreateRef<mcClock>() ));
	m_globals->define(Token(IDENTIFIER, "assert", {}, 0), CreateRef<Object>( CreateRef<mcAssert>() ));
	m_globals->define(Token(IDENTIFIER, "to_str", {}, 0), CreateRef<Object>( CreateRef<mcToString>() ));
	m_globals->define(Token(IDENTIFIER, "print",  {}, 0), CreateRef<Object>( CreateRef<mcPrint>() ));
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

Ref<Object> Interpreter::look_up_variable(const Token& name, const Expression& expression) {
	auto it = m_locals.find(&expression);
	if (it != m_locals.end()) {
		int distance = it->second;
		return m_environment->get_at(distance, name);
	}
	return m_globals->get(name);
}


void Interpreter::visit(const LiteralExpression& e) {
	m_result = CreateRef<Object>(e.value);
}

void Interpreter::visit(const GroupingExpression& e) {
	evaluate(e.expression);
}
void Interpreter::visit(const VariableExpression& e) {
	m_result = look_up_variable(e.name, e);
}
void Interpreter::visit(const AssignmentExpression& e) {
	Ref<Object> value = evaluate(e.value);
	Ref<Object> var;

	auto it = m_locals.find(&e);
	if (it != m_locals.end()) {
		int distance = it->second;
		var = m_environment->get_at(distance, e.name);
		var->value = value->value;
	} else {
		var = m_globals->get(e.name);
		var->value = value->value;
	}

	m_result = var;
}
void Interpreter::visit(const LogicalExpression& e) {
	Ref<Object> left = evaluate(e.left);

	if (e.operator_token.type == OR) {
		if (is_truthy(left)) {
			m_result = CreateRef<Object>(left);
			return;
		}
	} else if (e.operator_token.type == AND) {
		if (!is_truthy(left)) {
			m_result = CreateRef<Object>(left);
			return;
		}
	}

	m_result = CreateRef<Object>(evaluate(e.right));
}

void Interpreter::visit(const CallExpression& e) {
	Ref<Object> callee = evaluate(e.callee);

	std::vector<Ref<Object>> arguments;
	arguments.reserve(e.arguments.size());
	for (const Ref<Expression>& argument : e.arguments) {
		arguments.emplace_back(evaluate(argument));
	}

	if (!callee->is_callable()) {
		m_result = nullptr;
		throw InterpreterException(e.paren, "Object is not callable.");
	}

	const Ref<MinikCallable>& function = callee->as_callable();

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
	Ref<Object> object = evaluate(e.object);
	if (object->is_instance()) {
		m_result = object->as_instance()->get(e.name, object->as_instance());
		return;
	}

	throw InterpreterException(e.name, "Attempted to access property of a non-instance object.");
}
void Interpreter::visit(const SetExpression& e) {
	Ref<Object> object = evaluate(e.object);
	if (!object->is_instance()) {
		throw InterpreterException(e.name, "Attempted to access property of a non-instance object.");
	}

	Ref<Object> value = evaluate(e.value);
	object->as_instance()->set(e.name, CreateRef<Object>(value));
	m_result = value;
}

void Interpreter::visit(const ThisExpression& e) {
	m_result = look_up_variable(e.keyword, e);
}

void Interpreter::visit(const SubscriptExpression& e) {
	Ref<Object> object = evaluate(e.object);
	Ref<Object> key = evaluate(e.key);
	
	if (!key->is_double()) {
		throw InterpreterException(e.name, "List indices must be of type double.");
	}
	const double& index = key->as_double();

	if (object->is_list()) {
		const List& list = object->as_list();
		if (index < 0 || index >= list.size()) {
			throw InterpreterException(e.name, "List index out of bounds. The index " + std::to_string(index) + " is outside the valid range of 0 to " + std::to_string(list.size() - 1) + ".");
		}
		m_result = list.at(static_cast<size_t>(index));
		return;
	} else if (object->is_string()) {
		const std::string& str = object->as_string();
		if (index < 0 || index >= str.size()) {
			throw InterpreterException(e.name, "String index out of bounds. The index " + std::to_string(index) + " is outside the valid range of 0 to " + std::to_string(str.size() - 1) + ".");
		}
		m_result = CreateRef<Object>(str.substr(static_cast<size_t>(index),1));
		return;
	}

	throw InterpreterException(e.name, "Attempted to index a non-list or non-string type.");
}

void Interpreter::visit(const ArrayInitializerExpression& e) {
	List list = {};
	for (const auto& element : e.elements) {
		list.push_back(CreateRef<Object>(evaluate(element)));
	}
	m_result = CreateRef<Object>(list);
}

void Interpreter::visit(const SetSubscriptExpression& e) {
	Ref<Object> object = evaluate(e.object);
	Ref<Object> index = evaluate(e.index);
	Ref<Object> value = evaluate(e.value);

	if (!index->is_double()) {
		throw InterpreterException(e.name, "List indices must be of type double.");
	}
	if (index->as_double() < 0) {
		throw InterpreterException(e.name, "List index '"+std::to_string(index->as_double())+"' is out of bounds.");
	}
	const size_t idx = static_cast<size_t>(index->as_double());

	if (object->is_list()) {
		if (idx >= object->as_list().size()) {
			throw InterpreterException(e.name, "String index out of bounds. The index " + std::to_string(idx) + " is outside the valid range of 0 to " + std::to_string(object->as_list().size() - 1) + ".");
		}
		object->as_list().at(idx)->value = value->value;
		m_result = object;
		return;
	}

	if (object->is_string()) {
		std::string str = value->to_string();
		if (str.size() > 0) {
			object->as_string().at(idx) = str.at(0);
			m_result = object;
			return;
		}
	}

	throw InterpreterException(e.name, "Attempted to index a non-list or non-string type.");
}


void Interpreter::visit(const FunctionStatement& s) {
	Ref<MinikFunction> function = CreateRef<MinikFunction>(s, m_environment);
	m_environment->define(s.name, CreateRef<Object>(function));
}

void Interpreter::visit(const ReturnStatement& s) {
	Ref<Object> value;
	if (s.value) {
		value = evaluate(s.value);
	}
	throw ReturnException{value};
}

void Interpreter::visit(const ClassStatement& s) {
	Ref<Object> result = CreateRef<Object>(nullptr);
 	m_environment->define(s.name, result);

	MethodsMap methods(s.methods.size());
	for (const Ref<FunctionStatement>& method : s.methods) {
		bool is_initializer = (method->name.lexeme == s.name.lexeme);
		methods[method->name.lexeme] = CreateRef<MinikFunction>(*method.get(), m_environment, is_initializer);
	}

	MembersMap members(s.members.size());
	for (const Ref<VariableStatement>& member : s.members) {
		members[member->name.lexeme] = member;
	}

	result->value = CreateRef<MinikClass>(s.name.lexeme, methods, members);
}

void Interpreter::visit(const UnaryExpression& e) {
	Ref<Object> right = evaluate(e.right);

	switch (e.operator_token.type) {
		case BANG:
			m_result = CreateRef<Object>(!is_truthy(e.operator_token, right));
			return;
		case MINUS:
			if (right->is_double()) {
				m_result = CreateRef<Object>(-right->as_double());
				return;
			}
			throw InterpreterException(e.operator_token, *right.get(), "Invalid argument type to unary expression.");
		case PLUS_PLUS: {
			if (right->is_double()) {
				right->as_double()++;
				m_result = right;
				return;
			}
			throw InterpreterException(e.operator_token, *right.get(), "Invalid argument type to unary expression.");
		}
		case MINUS_MINUS: {
			if (right->is_double()) {
				right->as_double()--;
				m_result = right;
				return;
			}
			throw InterpreterException(e.operator_token, *right.get(), "Invalid argument type to unary expression.");
		}
		default:
			MN_ERROR("Unreachable. Interpreter visit unary");
			return;
	}
}

void Interpreter::visit(const BinaryExpression& e) {
	Ref<Object> left = evaluate(e.left);
	Ref<Object> right = evaluate(e.right);


	// string concatenation
	if (e.operator_token.type == PLUS && left->is_string() && right->is_string()) {
		m_result = CreateRef<Object>(left->as_string() + right->as_string());
		return;
	}


	// is equals
	switch (e.operator_token.type) {
		case EQUAL_EQUAL:
			m_result = CreateRef<Object>(is_equal(e.operator_token, left, right));
			return;
		case BANG_EQUAL:
			m_result = CreateRef<Object>(!is_equal(e.operator_token, left, right));
			return;
		default:
			break;
	}


	// doubles
	if (!left->is_double()) {
		throw InterpreterException(e.operator_token, left, "Invalid operand to binary expression.");
	}
	if (!right->is_double()) {
		throw InterpreterException(e.operator_token, right, "Invalid operand to binary expression.");
	}
	double l = left->as_double();
	double r = right->as_double();

	switch (e.operator_token.type) {
		case PLUS:
			m_result = CreateRef<Object>(l + r);
			return;
		case MINUS:
			m_result = CreateRef<Object>(l - r);
			return;
		case STAR:
			m_result = CreateRef<Object>(l * r);
			return;
		case MOD:
			m_result = CreateRef<Object>(double(int(l) % int(r)));
			return;
		case SLASH:
			m_result = CreateRef<Object>(l / r);
			return;
		case GREATER:
			m_result = CreateRef<Object>(l > r);
			return;
		case GREATER_EQUAL:
			m_result = CreateRef<Object>(l >= r);
			return;
		case LESS:
			m_result = CreateRef<Object>(l < r);
			return;
		case LESS_EQUAL:
			m_result = CreateRef<Object>(l <= r);
			return;
		default:
			return;
	}
}

Ref<Object> Interpreter::evaluate(const Ref<Expression>& expression) {
	expression->accept(*this);
	return m_result;
}

bool Interpreter::is_truthy(const Token& token, const Ref<Object>& object) const {
	if (object->is_nil()) {
		return false;
	}
	if (object->is_bool()) {
		return object->as_bool();
	}
	if (object->is_double()) {
		return object->as_double() != 0.0;
	}

	throw InterpreterException(token, object, "No viable conversion to bool.");
}
bool Interpreter::is_truthy(const Ref<Object>& object) const {
	if (object->is_nil()) {
		return false;
	}
	if (object->is_bool()) {
		return object->as_bool();
	}
	if (object->is_double()) {
		return object->as_double() != 0.0;
	}
	// TODO: throw exception ?
	return false;
}

bool Interpreter::is_equal(const Token& token, const Ref<Object>& a, const Ref<Object>& b) const {
	if (a->is_string() != b->is_string()) {
		throw InterpreterException(token, a->is_string() ? a : b, "Cannot compare a string with a non-string type.");
	}
	return a->equals(b);
}

void Interpreter::visit(const ExpressionStatement& s) {
	evaluate(s.expression);
}

void Interpreter::visit(const VariableStatement& s) {
	Ref<Object> value;
	if (s.initializer) {
		value = evaluate(s.initializer);
	}

	m_environment->define(s.name, value);
}

void Interpreter::visit(const BlockStatement& s) {
	execute_block(s.statements, CreateRef<Environment>(m_environment), s.deferred_statements);
}

void Interpreter::visit(const IfStatement& s) {
	if (is_truthy(evaluate(s.condition))) {
		execute_block(s.then_branch->statements, CreateRef<Environment>(m_environment), s.then_branch->deferred_statements);
	} else if (s.else_branch) {
		execute_block(s.else_branch->statements, CreateRef<Environment>(m_environment), s.else_branch->deferred_statements);
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
					execute_block(s.body->statements, CreateRef<Environment>(m_environment), s.body->deferred_statements);
				} catch (ContinueException) {
					// handles continue by ending execution of the block, jumping to the increment
				}
				if (s.increment) {
					evaluate(s.increment);
				}
				// clear deferred statements before next iteration
				s.body->deferred_statements.clear();
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

void Interpreter::execute_block(const std::vector<Ref<Statement>>& statements, const Ref<Environment>& environment, const std::vector<Ref<Statement>>& deferred_statements) {
	const Ref<Environment> previous = m_environment;
	try {
		m_environment = environment;
		for (auto& statement : statements) {
			execute(statement);
		}
	} catch (...) {
		//exit block
		if (deferred_statements.size() > 0) {
			for (auto it = deferred_statements.rbegin(); it != deferred_statements.rend(); ++it) {
				execute(*it);
			}
		}
		m_environment = previous;
		throw;
	}
	//exit block
	if (deferred_statements.size() > 0) {
		for (auto it = deferred_statements.rbegin(); it != deferred_statements.rend(); ++it) {
			execute(*it);
		}
	}
	m_environment = previous;
}

void Interpreter::visit(const DeferStatement& s) {
	s.enclosing_block->deferred_statements.push_back(s.statement);
}


}
