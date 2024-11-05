#include "class.h"
#include "exception.h"
#include "object.h"
#include "token.h"
#include "function.h"


namespace minik {


Object MinikClass::call(Interpreter& interpreter, const std::vector<Object>& arguments) {
	Ref<MinikInstance> instance = CreateRef<MinikInstance>(*this);

	Ref<MinikFunction> initializer = find_method(name);
	if (initializer) {
		initializer->bind(instance)->call(interpreter, arguments);
	}
	
	return { instance };
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

Object MinikInstance::get(const Token& name, const Ref<MinikInstance>& self) {
	auto it = fields.find(name.lexeme);
	if (it != fields.end()) {
		return *it->second.get();
	}

	Ref<MinikFunction> fn = clas.find_method(name.lexeme);
	if (fn) {
		return Object{fn->bind(self)};
	}

	throw InterpreterException(name, "Undefined property '"+name.lexeme+"' in '"+clas.name+"'.");
}

void MinikInstance::set(const Token& name, const Ref<Object>& value) {
	fields[name.lexeme] = value;
}


}
