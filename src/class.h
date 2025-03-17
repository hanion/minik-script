#pragma once

#include "callable.h"
#include <string>
#include <unordered_map>

namespace minik {

class MinikFunction;
class Environment;
class VariableStatement;
using MethodsMap = std::unordered_map<std::string, Ref<MinikFunction>>;
using CallablesMap = std::unordered_map<std::string, Ref<MinikCallable>>;
using MembersMap = std::unordered_map<std::string, Ref<VariableStatement>>;
using FieldsMap = std::unordered_map<std::string, Ref<Object>>;


class MinikClass : public MinikCallable {
public:
	MinikClass(const std::string name, const MethodsMap& methods, const MembersMap& members, const Ref<Environment>& closure)
		: name(name), methods(methods), members(members), m_closure(closure) {}

	virtual Ref<Object> call(Interpreter& interpreter, const std::vector<Ref<Object>>& arguments) override;
	virtual int arity() override;
	virtual std::string to_string() const override { return "<class " + name + ">"; }

	Ref<MinikFunction> find_method(const std::string& name);

public:
	const std::string name;
	MethodsMap methods;
	MembersMap members;
	Ref<Environment> m_closure;
};



class NativeClass : public MinikCallable {
public:
	NativeClass(const std::string& name) : name(name) {}

	Ref<MinikCallable> find_method(const std::string& name);

	void add_field(const std::string& name, const Ref<Object>& obj) {
		fields[name] = obj;
	}

	virtual int arity() override { return -1; }
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override;
	virtual std::string to_string() const override { return "<native class " + name + ">"; }

public:
	const std::string name;
	FieldsMap fields;
};


class MinikInstance {
public:
	MinikInstance(const MinikClass& clas) : clas(clas), nclas(""), is_native(false) {}
	MinikInstance(const NativeClass& nclas) : clas({"",{},{},nullptr}), nclas(nclas), is_native(true) {}

	Ref<Object> get(const Token& name, const Ref<MinikInstance>& self);
	void set(const Token& name, const Ref<Object>& value);


	std::string to_string() const { return "<instance of " + (is_native ? nclas.name : clas.name) + ">"; }

public:
	MinikClass clas;
	NativeClass nclas;
	bool is_native;
	
	FieldsMap fields = {};
};

class MinikNamespace {
public:
	MinikNamespace(const std::string& name, const Ref<MinikNamespace>& parent)
		: name(name), parent(parent) {}

	Ref<Object> get(const Token& name);

	const std::string to_string() const { return "<namespace " + name + ">"; }

	const std::string name;
	FieldsMap fields = {};
	Ref<MinikNamespace> parent;
};



}
