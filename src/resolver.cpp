#include "resolver.h"
#include "expression.h"
#include "minik.h"
#include "statement.h"
#include "token.h"

namespace minik {


void Resolver::resolve_block(const std::vector<Ref<Statement>>& statements) {
	for (const Ref<Statement>& statement : statements) {
		resolve(statement);
	}
}
void Resolver::resolve(const Ref<Statement>& statement) {
	statement->accept(*this);
}
void Resolver::resolve(const Ref<Expression>& expression) {
	expression->accept(*this);
}
void Resolver::resolve_local(const Expression& expression, const Token& token) {
	for (int i = m_scopes.size() - 1; i >= 0; i--) {
		if (m_scopes[i].count(token.lexeme) > 0) {
			m_interpreter.resolve(expression, m_scopes.size() - 1 - i);
			return;
		}
	}
}
void Resolver::resolve_function(const FunctionStatement& s, FunctionType type) {
	FunctionType enclosing_function = m_current_function;
	m_current_function = type;

	begin_scope();
	for (const Token& param : s.params) {
		declare(param);
		define(param);
	}
	resolve_block(s.body->statements);
	end_scope();

	m_current_function = enclosing_function;
}


void Resolver::begin_scope() {
	m_scopes.push_back({});
}
void Resolver::end_scope() {
	m_scopes.pop_back();
}

void Resolver::declare(const Token& name) {
	if (m_scopes.empty()) {
		return;
	}

	ResolverScope& scope = m_scopes.back();

	if (scope.count(name.lexeme) > 0) {
		report_error(name.line, "Variable with name '"+name.lexeme+"' already exists in this scope.");
	}

	scope[name.lexeme] = false;
}
void Resolver::define(const Token& name) {
	if (m_scopes.empty()) {
		return;
	}

	ResolverScope& scope = m_scopes.back();
	scope[name.lexeme] = true;
}


void Resolver::visit(const BlockStatement& s) {
	begin_scope();
	resolve_block(s.statements);
	end_scope();
}


void Resolver::visit(const VariableStatement& s) {
	declare(s.name);
	if (s.initializer) {
		resolve(s.initializer);
	}
	define(s.name);
}

void Resolver::visit(const FunctionStatement& s) {
	declare(s.name);
	define(s.name);
	resolve_function(s, FunctionType::FUNCTION);
}

void Resolver::visit(const ExpressionStatement& s) {
	resolve(s.expression);
}

void Resolver::visit(const IfStatement& s) {
	resolve(s.condition);
	resolve(s.then_branch);
	if (s.else_branch) {
		resolve(s.else_branch);
	}
}

void Resolver::visit(const PrintStatement& s) {
	resolve(s.expression);
}
void Resolver::visit(const ReturnStatement& s) {
	if (m_current_function != FunctionType::FUNCTION) {
		report_error(s.keyword.line, "Cannot return from outside of functions.");
	}
	if (s.value) {
		resolve(s.value);
	}
}
void Resolver::visit(const BreakStatement& s) {
	if (m_current_loop != LoopType::FOR) {
		report_error(s.keyword.line, "Cannot break from outside of loops.");
	}
}
void Resolver::visit(const ContinueStatement& s) {
	if (m_current_loop != LoopType::FOR) {
		report_error(s.keyword.line, "Cannot continue from outside of loops.");
	}
}

void Resolver::visit(const ForStatement& s) {
	LoopType enclosing_loop = m_current_loop;
	m_current_loop = LoopType::FOR;

	begin_scope();
	if (s.initializer) {
		resolve(s.initializer);
	}
	if (s.condition) {
		resolve(s.condition);
	}
	if (s.increment) {
		resolve(s.increment);
	}
	resolve(s.body);
	end_scope();

	m_current_loop = enclosing_loop;
}




void Resolver::visit(const VariableExpression& e) {
	if (!m_scopes.empty()) {
		auto it = m_scopes.back().find(e.name.lexeme);
		if (it != m_scopes.back().end()) {
			if (!it->second) {
				report_error(e.name.line, "Cannot read local variable '"+e.name.lexeme+"' in its own initializer.");
			}
		}
	}
	resolve_local(e, e.name);
}

void Resolver::visit(const AssignmentExpression& e) {
	resolve(e.value);
	resolve_local(e, e.name);
}
void Resolver::visit(const BinaryExpression& e) {
	resolve(e.left);
	resolve(e.right);
}
void Resolver::visit(const CallExpression& e) {
	resolve(e.callee);
	for (const Ref<Expression>& argument : e.arguments) {
		resolve(argument);
	}
}
void Resolver::visit(const GroupingExpression& e) {
	resolve(e.expression);
}
void Resolver::visit(const LogicalExpression& e) {
	resolve(e.left);
	resolve(e.right);
}
void Resolver::visit(const UnaryExpression& e) {
	resolve(e.right);
}

}

