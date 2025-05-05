#pragma once

#include "package.h"

namespace minik {

class ListPackage : public Package {
public:
	ListPackage() : Package("List") {}

	virtual void ImportPackage(const Ref<Environment>& environment, const std::string& as = "") override;
};

}

