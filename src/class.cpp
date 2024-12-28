#include "class.h"
#include "exception.h"
#include "object.h"
#include "token.h"
#include "function.h"
#include "interpreter.h"


namespace minik {


Ref<Object> MinikClass::call(Interpreter& interpreter, const std::vector<Ref<Object>>& arguments) {
	Ref<MinikInstance> instance = CreateRef<MinikInstance>(*this);

	for (const auto& member : members) {
		VariableStatement& vs = *member.second.get();
		Ref<Object> value;
		if (vs.initializer) {
			value = interpreter.evaluate(vs.initializer);
		}
		instance->fields[member.first] = value;
	}

	Ref<MinikFunction> initializer = find_method(name);
	if (initializer) {
		initializer->bind(instance)->call(interpreter, arguments);
	}
	
	return CreateRef<Object>(instance);
}
int MinikClass::arity() {
	Ref<MinikFunction> initializer = find_method(name);
	if (initializer) {
		return initializer->arity();
	}
	return 0;
}


Ref<MinikFunction> MinikClass::find_method(const std::string& name) {
	auto itm = methods.find(name);
	if (itm != methods.end()) {
		return itm->second;
	}
	return nullptr;
}

Ref<Object> MinikInstance::get(const Token& name, const Ref<MinikInstance>& self) {
	auto it = fields.find(name.lexeme);
	if (it != fields.end()) {
		return it->second;
	}

	Ref<MinikFunction> fn = clas.find_method(name.lexeme);
	if (fn) {
		return CreateRef<Object>(fn->bind(self));
	}

	throw InterpreterException(name, "Undefined property '"+name.lexeme+"' in '"+clas.name+"'.");
}

void MinikInstance::set(const Token& name, const Ref<Object>& value) {
	if (fields.count(name.lexeme) == 0) {
		throw InterpreterException(name, "Couldn't find field '"+name.lexeme+"' in instance.");
	}
	fields[name.lexeme] = value;
}


Ref<Object> MinikNamespace::get(const std::string& name) {
	auto field = fields.find(name);
	if (field != fields.end()) {
		return field->second;
	}
	auto method = methods.find(name);
	if (method != methods.end()) {
		return CreateRef<Object>(method->second);
	}

	auto callable = callables.find(name);
	if (callable != callables.end()) {
		return CreateRef<Object>(callable->second);
	}

	return nullptr;
}


}
