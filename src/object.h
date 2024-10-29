#pragma once

#include "base.h"
#include "callable.h"
#include "minik.h"
#include <cstddef>
#include <exception>
#include <variant>

namespace minik {

using Value = std::variant<std::nullptr_t, bool, double, std::string, Ref<MinikCallable>>;

struct Object {
	Value value;

	bool is_nil()      const { return std::holds_alternative<std::nullptr_t>(value); }
	bool is_bool()     const { return std::holds_alternative<bool>(value); }
	bool is_double()   const { return std::holds_alternative<double>(value); }
	bool is_string()   const { return std::holds_alternative<std::string>(value); }
	bool is_callable() const { return std::holds_alternative<Ref<MinikCallable>>(value); }

	const bool&        as_bool()   const { return std::get<bool>(value); }
	const double&      as_double() const { return std::get<double>(value); }
	const std::string& as_string() const { return std::get<std::string>(value); }
	const Ref<MinikCallable>& as_callable() const { return std::get<Ref<MinikCallable>>(value); }

	bool&        as_bool()   { return std::get<bool>(value); }
	double&      as_double() { return std::get<double>(value); }
	std::string& as_string() { return std::get<std::string>(value); }

	std::string to_string() const {
		if (is_nil()) {
			return "nil";
		} else if (is_bool()) {
			return as_bool() ? "true" : "false";
		} else if (is_double()) {
			return std::to_string(as_double());
		} else if (is_string()) {
			return as_string();
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

		// TODO: throw exception ?
		// MN_ERROR("Unreachable. Object::to_bool()");
		return false;
	}

	bool equals(const Object& other) const {
		if (is_nil() && other.is_nil()) {
			return true;
		}
		if (is_bool() && other.is_bool()) {
			return (as_bool() == other.as_bool());
		}
		if (is_double() && other.is_double()) {
			return (as_double() == other.as_double());
		}
		if (is_string() && other.is_string()) {
			return (as_string() == other.as_string());
		}
		return (to_bool() == other.to_bool());
	}
};

}
