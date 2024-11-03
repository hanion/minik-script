#pragma once

#include "base.h"
#include <string>
#include <vector>
namespace minik {

class Object;
class Interpreter;
class Token;

class MinikCallable {
public:
	virtual ~MinikCallable() = default;
	virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments);
	virtual int arity() { return 0; }
	virtual std::string to_string() const { return "<fn>"; }
};

class mcClock : public MinikCallable {
public:
	virtual int arity() override { return 0; }
	virtual std::string to_string() const override { return "<fn native clock>"; }
	virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
};

class mcAssert : public MinikCallable {
public:
	virtual int arity() override { return -1; }
	virtual std::string to_string() const override { return "<fn native assert>"; }
	virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
};

}

