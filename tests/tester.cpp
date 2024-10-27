#include "tester.h"
#include "log.h"
#include "minik.h"

Test::Test(std::filesystem::path test_path)
	: m_test_name(test_path.stem()),
	m_source_path(test_path),
	m_expected_dir(test_path.parent_path() / "expected"),
	m_result_path(m_expected_dir / (m_test_name + ".txt")) {

	if (!std::filesystem::exists(m_expected_dir)) {
		std::filesystem::create_directory(m_expected_dir);
	}
}

bool Test::run() {
	std::ostringstream ss;
	mn_output_stream = &ss;
	minik::run_file(m_source_path);
	m_output_string = ss.str();
	return true;
}

bool Test::compare_result() {
	if (not std::filesystem::exists(m_result_path)) {
		return save_result();
	}

	std::ifstream result_file(m_result_path);
	std::string result_content = std::string(
		(std::istreambuf_iterator<char>(result_file)),
		(std::istreambuf_iterator<char>())
	);

	if (result_content == m_output_string) {
		std::cout << " ✔ " << m_test_name << std::endl;
		return true;
	}

	std::cout << " ✘ " << m_test_name << std::endl;
	std::cout << "Test output:\n" << m_output_string << std::endl;
	std::cout << "Expected output:\n" << result_content << std::endl;
	std::cout << std::endl;
	return false;
}

bool Test::save_result() {
	std::ofstream result_file(m_result_path);
	result_file << m_output_string;
	std::cout << " ✔ " << m_test_name << " Result saved to " << m_result_path << std::endl;
	return true;
}


void Tester::run_all_tests() {
	int failed = 0;

	search_directory();
	for (Test& test : m_tests) {
		if (not test.run_test()) {
			failed++;
		}
	}

	if (failed) {
		std::cout << " ✘ Failed " << failed << " tests." << std::endl;
	} else {
		std::cout << " ✔ Passed " << m_tests.size() << " tests." << std::endl;
	}
}

void Tester::search_directory() {
	m_tests.clear();
	for (const auto& entry : std::filesystem::directory_iterator(m_search_path)) {
		std::filesystem::path path = entry.path();
		if (path.has_extension() and path.extension() == ".mn") {
			m_tests.push_back(Test(path));
		}
	}
}



