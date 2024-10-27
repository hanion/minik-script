#pragma once

#include <cstdio>
#include <filesystem>
#include <iostream>
#include <vector>
#include <fstream>

class Test {
public:
	Test(std::filesystem::path test_path);

	bool run_test()    { return run() and compare_result(); }
	bool record_test() { return run() and save_result(); }

private:
	bool run();
	bool compare_result();
	bool save_result();

public:
	const std::string m_test_name;
	const std::filesystem::path m_source_path;
	const std::filesystem::path m_expected_dir;
	const std::filesystem::path m_result_path;
	std::string m_output_string;
};


class Tester {
public:
	Tester(const std::filesystem::path& path) : m_search_path(path) {}

	void run_all_tests();

private:
	void search_directory();

private:
	const std::filesystem::path m_search_path;
	std::vector<Test> m_tests;
};

