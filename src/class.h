#pragma once

#include "callable.h"
#include <string>
#include <unordered_map>

namespace minik {

class MinikFunction;
class VariableStatement;
using MethodsMap = std::unordered_map<std::string, Ref<MinikFunction>>;
using MembersMap = std::unordered_map<std::string, Ref<VariableStatement>>;


class MinikClass : public MinikCallable {
public:
	MinikClass(const std::string name, const MethodsMap& methods, const MembersMap& members)
		: name(name), methods(methods), members(members) {}

	virtual Ref<Object> call(Interpreter& interpreter, const std::vector<Ref<Object>>& arguments) override;
	virtual int arity() override;
	virtual std::string to_string() const override { return "<class " + name + ">"; }

	Ref<MinikFunction> find_method(const std::string& name);

public:
	const std::string name;
	MethodsMap methods;
	MembersMap members;
};


class MinikInstance {
public:
	MinikInstance(const MinikClass& clas) : clas(clas) {}

	Ref<Object> get(const Token& name, const Ref<MinikInstance>& self);
	void set(const Token& name, const Ref<Object>& value);

	std::string to_string() const { return "<instance of " + clas.name + ">"; }

public:
	MinikClass clas;
	std::unordered_map<std::string, Ref<Object>> fields = {};
};

}
