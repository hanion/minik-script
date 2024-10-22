#include "expression.h"

namespace minik {

class AstPrinter : public Visitor {
public:
	virtual void visit(const LiteralExpression& literal)   override { result = literal.value.to_string(); }
	virtual void visit(const BinaryExpression& binary)     override { result = visit(binary.left) + " " + binary.operator_token.lexeme + " " + visit(binary.right); }
	virtual void visit(const UnaryExpression& unary)       override { result = unary.operator_token.lexeme + visit(unary.right); }
	virtual void visit(const GroupingExpression& grouping) override { result = "(" + visit(grouping.expression) + ")"; }

	std::string visit(const Ref<Expression>& expression) {
		expression->accept(*this);
		return result;
	}

	void print(const Ref<Expression>& expression) {
		MN_LOG(visit(expression).c_str());
	}

private:
	std::string result;
};

}
