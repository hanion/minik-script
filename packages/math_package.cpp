#include "math_package.h"

#include "class.h"
#include "interpreter.h"
#include "token.h"
#include <cmath>


namespace minik {

#define Math_PI 3.14159265358979323846f


#define DEFINE_AND_REGISTER_MATH(name) \
	DEFINE_FUNCTION(name,1,{return CreateRef<Object>(std::name(arguments[0]->as_double()));}); \
	m[#name] = CreateRef<Object>(CreateRef<mpc##name>());

#define DEFINE_AND_REGISTER_MATH_2(name) \
	DEFINE_FUNCTION(name,2,{return CreateRef<Object>(std::name(arguments[0]->as_double(), arguments[1]->as_double()));}); \
	m[#name] = CreateRef<Object>(CreateRef<mpc##name>());


void MathPackage::ImportPackage(const Ref<Environment>& environment, const std::string& as) {
	std::string package_import_as = name;
	if (!as.empty()) {
		package_import_as = as;
	}

	Ref<MinikNamespace> ns = CreateRef<MinikNamespace>(package_import_as, nullptr);
	FieldsMap& m = ns->fields;

	DEFINE_AND_REGISTER_MATH(sin);
	DEFINE_AND_REGISTER_MATH(cos);
	DEFINE_AND_REGISTER_MATH(sqrt);
	DEFINE_AND_REGISTER_MATH_2(min);
	DEFINE_AND_REGISTER_MATH_2(max);

	DEFINE_AND_REGISTER_MATH(acos);
	DEFINE_AND_REGISTER_MATH(asin);
	DEFINE_AND_REGISTER_MATH(atan);
	DEFINE_AND_REGISTER_MATH_2(atan2);
	DEFINE_AND_REGISTER_MATH(ceil);
	DEFINE_AND_REGISTER_MATH(cosh);
	DEFINE_AND_REGISTER_MATH(exp);
	DEFINE_AND_REGISTER_MATH(fabs);
	DEFINE_AND_REGISTER_MATH(floor);
	DEFINE_AND_REGISTER_MATH(log);
	DEFINE_AND_REGISTER_MATH(log10);
	DEFINE_AND_REGISTER_MATH_2(pow);
	DEFINE_AND_REGISTER_MATH(sinh);
	DEFINE_AND_REGISTER_MATH(tan);
	DEFINE_AND_REGISTER_MATH(tanh);

	m["PI"] = CreateRef<Object>(Math_PI);

	environment->predefine(Token(IDENTIFIER, ns->name, {}, 0), CreateRef<Object>(ns));
}


}

