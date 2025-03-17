#pragma once
#include "sqlite/sqlite3.h"
#include <string>

namespace minik {

class Database {
public:
	Database();
	~Database();

	void init(const std::string& db_file);
	void shutdown();

	void log_test_details(const std::string& name, const std::string& version, const std::string& description);
	void log_test_result(int test_id, int duration, const std::string& result, const std::string& output = "");
	void log_test_result(const std::string& test_name, int duration, const std::string& result, const std::string& output = "");


private:
	int get_test_id(const std::string& test_name);
	int exec_sql(const char* sql);
	std::string get_current_date();
	std::string escape_string(const std::string& input);

private:
	sqlite3* m_db = nullptr;
};
}





