#pragma once

#include "package.h"

namespace minik {

class MathPackage : public Package {
public:
	MathPackage() : Package("Math") {}

	virtual void ImportPackage(const Ref<Environment>& environment, const std::string& as = "") override;
};

}

