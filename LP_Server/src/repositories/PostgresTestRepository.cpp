#include "PostgresTestRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

PostgresTestRepository::PostgresTestRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Test> PostgresTestRepository::getTestsForCourse(int courseId) {
    std::lock_guard<std::mutex> lock(db.mutex());
    std::vector<Test> tests;

    std::string query = 
        "SELECT id, course_id, title FROM tests WHERE course_id = " + 
            std::to_string(courseId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get tests for course: " + error);
    }

    int rows = PQntuples(res);

    for (int i = 0; i < rows; ++i) {
        Test test;

        test.id = std::stoi(PQgetvalue(res, i, 0));
        test.courseId = std::stoi(PQgetvalue(res, i, 1));
        test.title = PQgetvalue(res, i, 2);
        
        tests.push_back(test);
    }

    PQclear(res);
    return tests;
}

Test PostgresTestRepository::createTest(int courseId, const std::string& title) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string courseIdValue = std::to_string(courseId);
    const char* params[] = {
        courseIdValue.c_str(),
        title.c_str()
    };

    PGresult* res = PQexecParams(
        db.get(),
        "INSERT INTO tests (course_id, title) "
        "VALUES ($1, $2) "
        "RETURNING id, course_id, title",
        2,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to create test: " + error);
    }

    Test test;
    test.id = std::stoi(PQgetvalue(res, 0, 0));
    test.courseId = std::stoi(PQgetvalue(res, 0, 1));
    test.title = PQgetvalue(res, 0, 2);
    PQclear(res);
    return test;
}
