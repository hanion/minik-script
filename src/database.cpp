#include "database.h"
#include "sqlite/sqlite3.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <ctime>

namespace minik {

Database::Database() {
}

Database::~Database() {
	shutdown();
}

void Database::init(const std::string& db_file) {
	if (sqlite3_open(db_file.c_str(), &m_db) != SQLITE_OK) {
		throw std::runtime_error("Failed to open database");
	}

	const char* create_test_details_table = R"(
		CREATE TABLE IF NOT EXISTS test_details (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		name TEXT NOT NULL UNIQUE,
		creation_date TEXT NOT NULL,
		minik_script_version TEXT NOT NULL,
		description TEXT
		);
	)";

	const char* create_test_results_table = R"(
		CREATE TABLE IF NOT EXISTS test_results (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		test_id INTEGER NOT NULL,
		date TEXT NOT NULL,
		duration INTEGER NOT NULL,
		result TEXT NOT NULL CHECK (result IN ('passed', 'failed')),
		output TEXT,
		FOREIGN KEY (test_id) REFERENCES test_details(id) ON DELETE CASCADE
		);
	)";

	if (exec_sql(create_test_details_table) != SQLITE_OK || exec_sql(create_test_results_table) != SQLITE_OK) {
		throw std::runtime_error("Failed to create tables");
	}
}

void Database::shutdown() {
	if (m_db) {
		sqlite3_close(m_db);
		m_db = nullptr;
	}
}

void Database::log_test_details(const std::string& name, const std::string& version, const std::string& description) {
	std::string creation_date = get_current_date();

	std::string query = R"(
		INSERT INTO test_details (name, creation_date, minik_script_version, description)
		VALUES (?, ?, ?, ?)
		ON CONFLICT(name)
		DO UPDATE SET
		creation_date = excluded.creation_date,
		minik_script_version = excluded.minik_script_version,
		description = excluded.description;
	)";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error("Failed to prepare statement");
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, creation_date.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, version.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, description.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		sqlite3_finalize(stmt);
		throw std::runtime_error("Failed to log test details");
	}

	sqlite3_finalize(stmt);
}

void Database::log_test_result(int test_id, int duration, const std::string& result, const std::string& output) {
	std::string date = get_current_date();

	const char* query = 
		"INSERT INTO test_results (test_id, date, duration, result, output) "
		"VALUES (?, ?, ?, ?, ?)";

	sqlite3_stmt* stmt;

	if (sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error("Failed to prepare statement");
	}

	sqlite3_bind_int(stmt, 1, test_id);
	sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, duration);
	sqlite3_bind_text(stmt, 4, result.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, output.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		sqlite3_finalize(stmt);
		throw std::runtime_error("Failed to insert test result");
	}

	sqlite3_finalize(stmt);
}

void Database::log_test_result(const std::string& test_name, int duration, const std::string& result, const std::string& output) {
	log_test_result(get_test_id(test_name), duration, result, output);
}


int Database::get_test_id(const std::string& test_name) {
	std::string query = "select id from test_details where name = ?";

	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		throw std::runtime_error("Failed to prepare statement");
	}

	sqlite3_bind_text(stmt, 1, test_name.c_str(), -1, SQLITE_STATIC);

	int test_id = -1;

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		test_id = sqlite3_column_int(stmt, 0);
	}

	sqlite3_finalize(stmt);

	if (test_id == -1) {
		throw std::runtime_error("Test not found");
	}

	return test_id;
}

int Database::exec_sql(const char* sql) {
	char* errMsg = nullptr;
	int rc = sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQLite error: " << errMsg << std::endl;
		sqlite3_free(errMsg);
	}
	return rc;
}

std::string Database::get_current_date() {
	time_t t = time(nullptr);
	struct tm tm;
	localtime_r(&t, &tm);
	char buffer[20];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
	return std::string(buffer);
}

std::string Database::escape_string(const std::string& input) {
	std::string result;
	for (char c : input) {
		if (c == '\'' || c == '\"' || c == '\\') {
			result += '\\';
		}
		result += c;
	}
	return result;
}
}




