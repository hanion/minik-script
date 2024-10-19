#include "base.h"
#include "minik.h"


int main(int argc, char* argv[]) {
	// NOTE: for debug:
	minik::run_file("../test.mn");
	return 0;
	if (argc > 2) {
		MN_ERROR("Usage: %s [script.mn]", argv[0]);
		return 64;
	} else if (argc == 2) {
		minik::run_file(argv[1]);
	} else {
		minik::run_prompt();
	}
}

