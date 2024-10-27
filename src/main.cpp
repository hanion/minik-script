#include "base.h"
#include "minik.h"
#include "tester.h"


int main(int argc, char* argv[]) {
	if (argc > 1 && std::string(argv[1]) == "--run-tests") {
		Tester tester = Tester("../tests/");
		tester.run_all_tests();
		return 0;
	}

	if (argc > 2) {
		MN_ERROR("Usage: %s [script.mn] or %s --run-tests", argv[0], argv[0]);
		return 64;
	} else if (argc == 2) {
		minik::run_file(argv[1]);
	} else {
		minik::run_prompt();
	}
}

