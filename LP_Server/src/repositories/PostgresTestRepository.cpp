#include "PostgresTestRepository.h"
#include <libpq-fe.h>

PostgresTestRepository::PostgresTestRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Test> PostgresTestRepository::getTestsForCourse(int courseId) {
    std::vector<Test> tests;

    std::string query = 
        "SELECT id, course_id, title FROM tests WHERE course_id = " + 
            std::to_string(courseId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());

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