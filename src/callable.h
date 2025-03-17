#pragma once

#include "base.h"
#include <string>
#include <vector>
namespace minik {

class Object;
class Interpreter;
class Token;
class MinikInstance;

using Arguments = std::vector<Ref<Object>>;

class MinikCallable {
public:
	virtual ~MinikCallable() = default;
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments);
	virtual int arity() { return 0; }
	virtual std::string to_string() const { return "<fn>"; }

	MinikInstance* instance = nullptr;
};

class mcClock : public MinikCallable {
public:
	virtual int arity() override { return 0; }
	virtual std::string to_string() const override { return "<fn native clock>"; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override;
};

class mcAssert : public MinikCallable {
public:
	virtual int arity() override { return -1; }
	virtual std::string to_string() const override { return "<fn native assert>"; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override;
};

class mcToString : public MinikCallable {
public:
	virtual int arity() override { return 1; }
	virtual std::string to_string() const override { return "<fn native to_str>"; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override;
};

class mcPrint : public MinikCallable {
public:
	virtual int arity() override { return -1; }
	virtual std::string to_string() const override { return "<fn native print>"; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override;
};

}

