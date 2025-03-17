#pragma once

#include "class.h"
#include "environment.h"
#include "log.h"

namespace minik {

class Package {
public:
	Package(std::string name) : name(name) {}

	virtual void ImportPackage(const Ref<Environment>& environment, const std::string& as = "") {}

	std::string name;
};


}
