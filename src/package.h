#pragma once

#include "class.h"
#include "environment.h"
#include "log.h"

namespace minik {

#define DEFINE_FUNCTION(name, arit, body)  \
class mpc##name : public MinikCallable { public:  \
	virtual int arity() override { return arit; }\
	virtual std::string to_string() const override { return "<fn native " #name ">"; } \
	virtual Ref<Object> call(Interpreter& interpreter, const Arguments& arguments) override { body } };

#define DEFINE_AND_REGISTER(name, arit, body) \
	DEFINE_FUNCTION(name,arit,body) \
	m[#name] = CreateRef<Object>(CreateRef<mpc##name>());


class Package {
public:
	Package(std::string name) : name(name) {}

	virtual void ImportPackage(const Ref<Environment>& environment, const std::string& as = "") {}

	std::string name;
};


}
