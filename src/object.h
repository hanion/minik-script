#pragma once

#include "base.h"
#include "callable.h"
#include "class.h"
#include "minik.h"
#include <cstddef>
#include <exception>
#include <variant>

namespace minik {

using List = std::vector<Ref<Object>>;
using Value = std::variant<std::nullptr_t, bool, double, std::string, Ref<MinikCallable>, Ref<MinikInstance>, List, Ref<MinikNamespace>>;

struct Object {
	Value value = {nullptr};

	Object() : value(nullptr) {}
	Object(void*) : value(nullptr) {}
	Object(bool val) : value(val) {}
	Object(double val) : value(val) {}
	Object(std::string val) : value(val) {}
	Object(Ref<MinikCallable> val) : value(val) {}
	Object(Ref<MinikInstance> val) : value(val) {}
	Object(Ref<MinikNamespace> val) : value(val) {}
	Object(List val) : value(val) {}

	Object(Object const &val) : value(val.value) {}
	Object(const Ref<Object>& val) : value(val->value) {}

	bool is_nil()      const { return std::holds_alternative<std::nullptr_t>(value); }
	bool is_bool()     const { return std::holds_alternative<bool>(value); }
	bool is_double()   const { return std::holds_alternative<double>(value); }
	bool is_string()   const { return std::holds_alternative<std::string>(value); }
	bool is_list()     const { return std::holds_alternative<List>(value); }
	bool is_callable() const { return std::holds_alternative<Ref<MinikCallable>>(value); }
	bool is_instance() const { return std::holds_alternative<Ref<MinikInstance>>(value); }
	bool is_namespace()const { return std::holds_alternative<Ref<MinikNamespace>>(value); }

	const bool&        as_bool()   const { return std::get<bool>(value); }
	const double&      as_double() const { return std::get<double>(value); }
	const std::string& as_string() const { return std::get<std::string>(value); }
	const List&        as_list()   const { return std::get<List>(value); }
	const Ref<MinikCallable>& as_callable() const { return std::get<Ref<MinikCallable>>(value); }
	const Ref<MinikInstance>& as_instance() const { return std::get<Ref<MinikInstance>>(value); }
	const Ref<MinikNamespace>& as_namespace() const { return std::get<Ref<MinikNamespace>>(value); }

	bool&        as_bool()   { return std::get<bool>(value); }
	double&      as_double() { return std::get<double>(value); }
	std::string& as_string() { return std::get<std::string>(value); }
	List&        as_list()   { return std::get<List>(value); }

	std::string to_string() const {
		if (is_nil()) {
			return "nil";
		} else if (is_bool()) {
			return as_bool() ? "true" : "false";
		} else if (is_double()) {
			return std::to_string(as_double());
		} else if (is_string()) {
			return as_string();
		} else if (is_callable()) {
			return as_callable()->to_string();
		} else if (is_instance()) {
			return as_instance()->to_string();
		} else if (is_namespace()) {
			return as_namespace()->to_string();
		} else if (is_list()) {
			std::string txt = "<list>";
			const List& list = as_list();
			for (size_t i = 0; i < list.size(); ++i) {
				const Ref<Object>& element = list.at(i);
				txt += element->to_string();
				if (i != list.size()-1) {
					txt += ", ";
				}
			}

			txt += "</list size="+std::to_string(list.size())+">";
			return txt;
		}
		return "";
	}

	bool to_bool() const {
		if (is_nil()) {
			return false;
		}
		if (is_bool()) {
			return as_bool();
		}
		if (is_double()) {
			return (as_double() != 0.0);
		}
		if (is_list()) {
			return as_list().empty();
		}

		// TODO: throw exception ?
		// MN_ERROR("Unreachable. Object::to_bool()");
		return false;
	}

	bool equals(const Ref<Object>& other) const {
		if (is_nil() && other->is_nil()) {
			return true;
		}
		if (is_bool() && other->is_bool()) {
			return (as_bool() == other->as_bool());
		}
		if (is_double() && other->is_double()) {
			return (as_double() == other->as_double());
		}
		if (is_string() && other->is_string()) {
			return (as_string() == other->as_string());
		}
		return (to_bool() == other->to_bool());
	}
};

}
