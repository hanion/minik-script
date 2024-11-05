#pragma once

#include "callable.h"
#include <string>
#include <unordered_map>

namespace minik {

class MinikFunction;
using MethodsMap = std::unordered_map<std::string, Ref<MinikFunction>>;


class MinikClass : public MinikCallable {
public:
	MinikClass(const std::string name, const MethodsMap& methods) : name(name), methods(methods) {}

	virtual Object call(Interpreter& interpreter, const std::vector<Object>& arguments) override;
	virtual int arity() override;
	virtual std::string to_string() const override { return "<class " + name + ">"; }

	Ref<MinikFunction> find_method(const std::string& name);

public:
	const std::string name;
	MethodsMap methods;
};


class MinikInstance {
public:
	MinikInstance(const MinikClass& clas) : clas(clas) {}

	Object get(const Token& name, const Ref<MinikInstance>& self);
	void set(const Token& name, const Ref<Object>& value);

	std::string to_string() const { return "<instance of " + clas.name + ">"; }

public:
	MinikClass clas;
	std::unordered_map<std::string, Ref<Object>> fields = {};
};

}
