#pragma once
#include "base.h"

namespace minik {

void run(const std::string& source);
void run_file(const std::string& filename);
void run_prompt();
void report_error(int line, const std::string& message);


}

