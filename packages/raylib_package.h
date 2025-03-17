#pragma once

#include "package.h"

namespace minik {

class RaylibPackage : public Package {
public:
	RaylibPackage() : Package("Raylib") {}

	virtual void ImportPackage(const Ref<Environment>& environment, const std::string& as = "") override;
};

}

