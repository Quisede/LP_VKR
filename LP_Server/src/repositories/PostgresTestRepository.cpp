#include "PostgresTestRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

PostgresTestRepository::PostgresTestRepository(PostgresConnection& connection)
    : db(connection) {
    ensureSchema();
}

namespace {
Test testFromRow(PGresult* res, int row) {
    Test test;
    test.id = std::stoi(PQgetvalue(res, row, 0));
    test.courseId = std::stoi(PQgetvalue(res, row, 1));
    test.title = PQgetvalue(res, row, 2);
    test.status = PQgetvalue(res, row, 3);
    test.deadlineAt = PQgetisnull(res, row, 4) ? "" : PQgetvalue(res, row, 4);
    test.available = std::string(PQgetvalue(res, row, 5)) == "t";
    test.maxAttempts = std::stoi(PQgetvalue(res, row, 6));
    test.timeLimitMinutes = std::stoi(PQgetvalue(res, row, 7));
    test.attemptsUsed = std::stoi(PQgetvalue(res, row, 8));
    test.passed = std::string(PQgetvalue(res, row, 9)) == "t";
    test.bestPercentage = std::stod(PQgetvalue(res, row, 10));
    test.canAttempt = std::string(PQgetvalue(res, row, 11)) == "t";
    return test;
}

const char* kTestSelectFields =
    "id, course_id, title, status, "
    "COALESCE(to_char(deadline_at, 'YYYY-MM-DD\"T\"HH24:MI:SS'), '') AS deadline_at, "
    "(status = 'active' AND (deadline_at IS NULL OR deadline_at > CURRENT_TIMESTAMP)) AS available, "
    "max_attempts, "
    "time_limit_minutes, "
    "0 AS attempts_used, "
    "false AS passed, "
    "0.0 AS best_percentage, "
    "(status = 'active' AND (deadline_at IS NULL OR deadline_at > CURRENT_TIMESTAMP)) AS can_attempt ";
}

void PostgresTestRepository::ensureSchema() {
    std::lock_guard<std::mutex> lock(db.mutex());

    PGresult* res = PQexec(
        db.get(),
        "ALTER TABLE tests ADD COLUMN IF NOT EXISTS status TEXT NOT NULL DEFAULT 'active';"
        "ALTER TABLE tests ADD COLUMN IF NOT EXISTS deadline_at TIMESTAMP NULL;"
        "ALTER TABLE tests ADD COLUMN IF NOT EXISTS max_attempts INTEGER NOT NULL DEFAULT 0;"
        "ALTER TABLE tests ADD COLUMN IF NOT EXISTS time_limit_minutes INTEGER NOT NULL DEFAULT 30;"
        "ALTER TABLE tests DROP CONSTRAINT IF EXISTS tests_status_check;"
        "ALTER TABLE tests ADD CONSTRAINT tests_status_check CHECK (status IN ('active', 'closed'));"
        "ALTER TABLE tests DROP CONSTRAINT IF EXISTS tests_max_attempts_check;"
        "ALTER TABLE tests ADD CONSTRAINT tests_max_attempts_check CHECK (max_attempts >= 0);"
        "ALTER TABLE tests DROP CONSTRAINT IF EXISTS tests_time_limit_minutes_check;"
        "ALTER TABLE tests ADD CONSTRAINT tests_time_limit_minutes_check CHECK (time_limit_minutes BETWEEN 1 AND 300);");

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to ensure tests schema: " + error);
    }

    PQclear(res);
}

std::vector<Test> PostgresTestRepository::getTestsForCourse(int courseId) {
    std::lock_guard<std::mutex> lock(db.mutex());
    std::vector<Test> tests;

    std::string query =
        "SELECT " + std::string(kTestSelectFields) +
        "FROM tests WHERE course_id = " +
            std::to_string(courseId) + " ORDER BY id;";

    PGresult* res = PQexec(db.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get tests for course: " + error);
    }

    int rows = PQntuples(res);

    for (int i = 0; i < rows; ++i) {
        tests.push_back(testFromRow(res, i));
    }

    PQclear(res);
    return tests;
}

std::vector<Test> PostgresTestRepository::getTestsForCourseForStudent(int courseId, int userId) {
    std::lock_guard<std::mutex> lock(db.mutex());
    std::vector<Test> tests;

    const std::string courseIdValue = std::to_string(courseId);
    const std::string userIdValue = std::to_string(userId);
    const char* params[] = {courseIdValue.c_str(), userIdValue.c_str()};

    PGresult* res = PQexecParams(
        db.get(),
        "SELECT "
        "t.id, t.course_id, t.title, t.status, "
        "COALESCE(to_char(t.deadline_at, 'YYYY-MM-DD\"T\"HH24:MI:SS'), '') AS deadline_at, "
        "(t.status = 'active' AND (t.deadline_at IS NULL OR t.deadline_at > CURRENT_TIMESTAMP)) AS available, "
        "t.max_attempts, "
        "t.time_limit_minutes, "
        "COUNT(a.id)::int AS attempts_used, "
        "COALESCE(BOOL_OR(a.passed), false) AS passed, "
        "COALESCE(MAX(a.percentage), 0.0) AS best_percentage, "
        "((t.status = 'active' AND (t.deadline_at IS NULL OR t.deadline_at > CURRENT_TIMESTAMP)) "
        " AND (t.max_attempts = 0 OR COUNT(a.id) < t.max_attempts)) AS can_attempt "
        "FROM tests t "
        "LEFT JOIN attempts a ON a.test_id = t.id AND a.user_id = $2 "
        "WHERE t.course_id = $1 "
        "GROUP BY t.id, t.course_id, t.title, t.status, t.deadline_at, t.max_attempts, t.time_limit_minutes "
        "ORDER BY t.id",
        2,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get student tests for course: " + error);
    }

    const int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        tests.push_back(testFromRow(res, i));
    }

    PQclear(res);
    return tests;
}

std::optional<Test> PostgresTestRepository::getTestById(int testId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string testIdValue = std::to_string(testId);
    const char* params[] = {testIdValue.c_str()};

    const std::string query = "SELECT " + std::string(kTestSelectFields) + "FROM tests WHERE id = $1";
    PGresult* res = PQexecParams(
        db.get(),
        query.c_str(),
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get test by id: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    Test test = testFromRow(res, 0);
    PQclear(res);
    return test;
}

Test PostgresTestRepository::createTest(int courseId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string courseIdValue = std::to_string(courseId);
    const std::string maxAttemptsValue = std::to_string(maxAttempts);
    const std::string timeLimitValue = std::to_string(timeLimitMinutes);
    const char* params[] = {
        courseIdValue.c_str(),
        title.c_str(),
        status.c_str(),
        deadlineAt.empty() ? nullptr : deadlineAt.c_str(),
        maxAttemptsValue.c_str(),
        timeLimitValue.c_str()
    };
    int lengths[] = {0, 0, 0, 0, 0, 0};
    int formats[] = {0, 0, 0, 0, 0, 0};

    PGresult* res = PQexecParams(
        db.get(),
        "INSERT INTO tests (course_id, title, status, deadline_at, max_attempts, time_limit_minutes) "
        "VALUES ($1, $2, $3, NULLIF($4, '')::timestamp, $5::int, $6::int) "
        "RETURNING id, course_id, title, status, "
        "          COALESCE(to_char(deadline_at, 'YYYY-MM-DD\"T\"HH24:MI:SS'), '') AS deadline_at, "
        "          (status = 'active' AND (deadline_at IS NULL OR deadline_at > CURRENT_TIMESTAMP)) AS available, "
        "          max_attempts, time_limit_minutes, 0 AS attempts_used, false AS passed, 0.0 AS best_percentage, "
        "          (status = 'active' AND (deadline_at IS NULL OR deadline_at > CURRENT_TIMESTAMP)) AS can_attempt",
        6,
        nullptr,
        params,
        lengths,
        formats,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to create test: " + error);
    }

    Test test = testFromRow(res, 0);
    PQclear(res);
    return test;
}

Test PostgresTestRepository::updateTest(int testId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string testIdValue = std::to_string(testId);
    const std::string maxAttemptsValue = std::to_string(maxAttempts);
    const std::string timeLimitValue = std::to_string(timeLimitMinutes);
    const char* params[] = {
        title.c_str(),
        status.c_str(),
        deadlineAt.empty() ? nullptr : deadlineAt.c_str(),
        maxAttemptsValue.c_str(),
        timeLimitValue.c_str(),
        testIdValue.c_str()
    };
    int lengths[] = {0, 0, 0, 0, 0, 0};
    int formats[] = {0, 0, 0, 0, 0, 0};

    PGresult* res = PQexecParams(
        db.get(),
        "UPDATE tests SET title = $1, status = $2, deadline_at = NULLIF($3, '')::timestamp, max_attempts = $4::int, time_limit_minutes = $5::int WHERE id = $6 "
        "RETURNING id, course_id, title, status, "
        "          COALESCE(to_char(deadline_at, 'YYYY-MM-DD\"T\"HH24:MI:SS'), '') AS deadline_at, "
        "          (status = 'active' AND (deadline_at IS NULL OR deadline_at > CURRENT_TIMESTAMP)) AS available, "
        "          max_attempts, time_limit_minutes, 0 AS attempts_used, false AS passed, 0.0 AS best_percentage, "
        "          (status = 'active' AND (deadline_at IS NULL OR deadline_at > CURRENT_TIMESTAMP)) AS can_attempt",
        6,
        nullptr,
        params,
        lengths,
        formats,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to update test: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        throw std::runtime_error("Test not found");
    }

    Test test = testFromRow(res, 0);
    PQclear(res);
    return test;
}

void PostgresTestRepository::deleteTest(int testId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string testIdValue = std::to_string(testId);
    const char* params[] = {testIdValue.c_str()};

    PGresult* res = PQexecParams(
        db.get(),
        "DELETE FROM tests WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to delete test: " + error);
    }

    PQclear(res);
}
