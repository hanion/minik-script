#include "class.h"
#include "callable.h"
#include "exception.h"
#include "log.h"
#include "object.h"
#include "token.h"
#include "function.h"
#include "interpreter.h"


namespace minik {


Ref<Object> MinikClass::call(Interpreter& interpreter, const std::vector<Ref<Object>>& arguments) {
	Ref<MinikInstance> instance = CreateRef<MinikInstance>(*this);

	const Ref<Environment> previous = interpreter.m_environment;
	interpreter.m_environment = m_closure;

	for (const auto& member : members) {
		VariableStatement& vs = *member.second.get();
		Ref<Object> value;
		if (vs.initializer) {
			value = interpreter.evaluate(vs.initializer);
		}
		instance->fields[member.first] = value;
	}
	interpreter.m_environment = previous;

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

	if (!is_native) {
		Ref<MinikFunction> fn = clas.find_method(name.lexeme);
		if (fn) {
			return CreateRef<Object>(fn->bind(self));
		}
	} else {
		MN_LOG("native get %s in %s", name.lexeme.c_str(), nclas.name.c_str());
		Ref<MinikCallable> fn = nclas.find_method(name.lexeme);
		if (fn) {
			return CreateRef<Object>(fn);
		}
	}

	throw InterpreterException(name, "Undefined property '"+name.lexeme+"' in '"+clas.name+"'.");
}

void MinikInstance::set(const Token& name, const Ref<Object>& value) {
	if (fields.count(name.lexeme) == 0) {
		throw InterpreterException(name, "Couldn't find field '"+name.lexeme+"' in instance.");
	}
	fields[name.lexeme] = value;
}


Ref<Object> MinikNamespace::get(const Token& name) {
	auto field = fields.find(name.lexeme);
	if (field != fields.end()) {
		return field->second;
	}

	if (parent) {
		return parent->get(name);
	}

	// FIX: we cant fail everytime, because sometimes we are just checking if it exists or not
	// throw InterpreterException(name, "Couldn't find field '"+name.lexeme+"' in namespace.");
	return nullptr;
}





Ref<Object> NativeClass::call(Interpreter& interpreter, const Arguments& arguments) {
	Ref<MinikInstance> instance = CreateRef<MinikInstance>(*this);

	for (const auto& field : fields) {
		instance->fields[field.first] = field.second;
	}

	Ref<Object> result;

	Ref<MinikCallable> initializer = find_method(name);
	if (initializer) {
		if (MinikFunction* f = dynamic_cast<MinikFunction*>(initializer.get())) {
			result = f->bind(instance)->call(interpreter, arguments);
		} else {
			result = initializer->call(interpreter, arguments);
		}
	} else {
		result = CreateRef<Object>(instance);
	}
	
	return result;
}
Ref<MinikCallable> NativeClass::find_method(const std::string& name) {
	auto it = fields.find(name);
	if (it != fields.end()) {
		if (it->second->is_callable()) {
			return it->second->as_callable();
		}
	}
	return nullptr;
}

}
