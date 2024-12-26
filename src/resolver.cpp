#include "resolver.h"
#include "expression.h"
#include "log.h"
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

	BlockStatement* enclosing_block = m_current_block;
	m_current_block = s.body.get();
	
	begin_scope();
	for (const Token& param : s.params) {
		declare(param);
		define(param);
	}
	resolve_block(s.body->statements);
	end_scope();

	m_current_block = enclosing_block;
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

	scope[name.lexeme] = SymbolState::DECLARED;
}
void Resolver::define(const std::string& name) {
	if (m_scopes.empty()) {
		return;
	}

	ResolverScope& scope = m_scopes.back();
	scope[name] = SymbolState::DEFINED;
}


void Resolver::visit(const BlockStatement& s) {
	BlockStatement* enclosing_block = m_current_block;
	m_current_block = const_cast<BlockStatement*>(&s);

	begin_scope();
	resolve_block(s.statements);
	end_scope();

	m_current_block = enclosing_block;
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

void Resolver::visit(const ReturnStatement& s) {
	if (m_current_function != FunctionType::FUNCTION) {
		report_error(s.keyword.line, "Cannot return from outside of functions.");
	}
	if (s.value) {
		if (m_current_function == FunctionType::INITIALIZER) {
			report_error(s.keyword.line, "Cannot return a value from initializer.");
		}
		resolve(s.value);
	}
}

bool Resolver::label_exists(const Token& label) {
	for (int i = m_scopes.size() - 1; i >= 0; i--) {
		if (m_scopes[i].count(label.lexeme) > 0) {
			return m_scopes[i].at(label.lexeme) == SymbolState::LABEL;
		}
	}
	return false;
}
void Resolver::visit(const BreakStatement& s) {
	if (m_current_loop != LoopType::FOR) {
		report_error(s.keyword.line, "Invalid 'break' statement: 'break' can only be used inside a loop.");
	}
	if (s.keyword.type == IDENTIFIER) {
		if (!label_exists(s.keyword)) {
			report_error(s.keyword.line, "Invalid 'break' label: The label '"+s.keyword.lexeme+"' does not exist in the current scope.");
		}
	}
}
void Resolver::visit(const ContinueStatement& s) {
	if (m_current_loop != LoopType::FOR) {
		report_error(s.keyword.line, "Invalid 'continue' statement: 'continue' can only be used inside a loop.");
	}
	if (s.keyword.type == IDENTIFIER) {
		if (!label_exists(s.keyword)) {
			report_error(s.keyword.line, "Invalid 'continue' label: The label '"+s.keyword.lexeme+"' does not exist in the current scope.");
		}
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

void Resolver::visit(const ClassStatement& s) {
	ClassType enclosing_class = m_current_class;
	m_current_class = ClassType::CLASS;

	declare(s.name);
	define(s.name);

	begin_scope();
	define("this");

	for (const Ref<FunctionStatement>& method : s.methods) {
		FunctionType declaration = FunctionType::METHOD;
		if (method->name.lexeme == s.name.lexeme) {
			declaration = FunctionType::INITIALIZER;
		}
		resolve_function(*method.get(), declaration);
	}

	end_scope();

	m_current_class = enclosing_class;
}

void Resolver::visit(const DeferStatement& s) {
	if (m_current_block == nullptr) {
		report_error(s.token.line, "'defer' can only be used inside a block.");
		return;
	}
	const_cast<DeferStatement*>(&s)->enclosing_block = m_current_block;
	resolve(s.statement);
}

void Resolver::visit(const LabelStatement& s) {
	if (m_scopes.size() > 0)  {
		ResolverScope& scope = m_scopes.back();
		if (scope.count(s.name.lexeme) > 0) {
			report_error(s.name.line, "Variable with name '"+s.name.lexeme+"' already exists in this scope.");
		}
		scope[s.name.lexeme] = SymbolState::LABEL;
	}

	if (s.loop) {
		resolve(s.loop);
	}
}

void Resolver::visit(const GotoStatement& s) {
}



void Resolver::visit(const VariableExpression& e) {
	if (!m_scopes.empty()) {
		auto it = m_scopes.back().find(e.name.lexeme);
		if (it != m_scopes.back().end()) {
			if (it->second != SymbolState::DEFINED) {
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
void Resolver::visit(const GetExpression& e) {
	resolve(e.object);
}
void Resolver::visit(const SetExpression& e) {
	resolve(e.value);
	resolve(e.object);
}
void Resolver::visit(const ThisExpression& e) {
	if (m_current_class == ClassType::NONE) {
		report_error(e.keyword.line, "Cannot use 'this' outside of a class.");
		return;
	}
	resolve_local(e, e.keyword);
}

void Resolver::visit(const SubscriptExpression& e) {
	resolve(e.object);
	resolve(e.key);
}

void Resolver::visit(const ArrayInitializerExpression& e) {
	for (const auto& element : e.elements) {
		resolve(element);
	}
}

void Resolver::visit(const SetSubscriptExpression& e) {
	resolve(e.object);
	resolve(e.index);
	resolve(e.value);
}

}

