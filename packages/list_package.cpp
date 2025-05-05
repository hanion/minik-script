#include "list_package.h"

#include "class.h"
#include "package.h"
#include "token.h"
#include <cmath>


namespace minik {

void ListPackage::ImportPackage(const Ref<Environment>& environment, const std::string& as) {
	std::string package_import_as = name;
	if (!as.empty()) {
		package_import_as = as;
	}

	Ref<MinikNamespace> ns = CreateRef<MinikNamespace>(package_import_as, nullptr);
	FieldsMap& m = ns->fields;


	DEFINE_AND_REGISTER(size, 1, {
		return CreateRef<Object>((double)arguments[0]->as_list().size());
	});
	DEFINE_AND_REGISTER(count, 1, {
		return CreateRef<Object>((double)arguments[0]->as_list().size());
	});


	DEFINE_AND_REGISTER(clear, 1, {
		arguments[0]->as_list().clear();
		return nullptr;
	});

	DEFINE_AND_REGISTER(pop_back, 1, {
		arguments[0]->as_list().pop_back();
		return nullptr;
	});
	DEFINE_AND_REGISTER(pop, 1, {
		arguments[0]->as_list().pop_back();
		return nullptr;
	});

	DEFINE_AND_REGISTER(push_back, 2, {
		arguments[0]->as_list().push_back(arguments[1]);
		return nullptr;
	});
	DEFINE_AND_REGISTER(push, 2, {
		arguments[0]->as_list().push_back(arguments[1]);
		return nullptr;
	});
	DEFINE_AND_REGISTER(append, 2, {
		arguments[0]->as_list().push_back(arguments[1]);
		return nullptr;
	});

	DEFINE_AND_REGISTER(back, 1, {
		if (arguments[0]->as_list().empty()) { return nullptr; }
		return arguments[0]->as_list().back();
	});
	DEFINE_AND_REGISTER(front, 1, {
		if (arguments[0]->as_list().empty()) { return nullptr; }
		return arguments[0]->as_list().front();
	});

	DEFINE_AND_REGISTER(deep_copy, 1, {
		Ref<Object> new_object = CreateRef<Object>();
		if (arguments[0]->is_list()) {
			const List& ol = arguments[0]->as_list();
			List nl = List();
			for (size_t i = 0; i < ol.size(); ++i) {
				nl.push_back(CreateRef<Object>(ol[i]));
			}
			new_object->value = nl;
		}
		return new_object;
	});

	environment->predefine(Token(IDENTIFIER, ns->name, {}, 0), CreateRef<Object>(ns));
}


}

