#include "PostgresEnrollmentRepository.h"
#include <stdexcept>
#include <libpq-fe.h>
#include <stdexcept>
#include <string>

PostgresEnrollmentRepository::PostgresEnrollmentRepository(PostgresConnection& conn) 
    : connection(conn) {}

void PostgresEnrollmentRepository::enrollStudent(int studentId, int courseId) {
    std::string studentIdStr = std::to_string(studentId);
    std::string courseIdStr = std::to_string(courseId);

    const char* paramValues[2] = {
        studentIdStr.c_str(),
        courseIdStr.c_str()
    };

    PGresult* res = PQexecParams(
        connection.get(),
        "INSERT INTO enrollments(student_id, course_id) VALUES ($1, $2)",
        2, // number of parameters
        nullptr, // parameter types (can be null for default)
        paramValues,
        nullptr, // parameter lengths (can be null for text)
        nullptr, // parameter formats (can be null for text)
        0 // result format (0 for text)
    );

    if(PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to enroll student: " + error);
    }

    PQclear(res);
}

std::vector<int> PostgresEnrollmentRepository::getCoursesForStudent(int studentId) {
    std::vector<int> courseIds;

    std::string studentIdStr = std::to_string(studentId);

    const char* paramValues[1] = { 
        studentIdStr.c_str() 
    };

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT course_id FROM enrollments WHERE student_id = $1 ORDER BY course_id",
        1, // number of parameters
        nullptr, // parameter types (can be null for default)
        paramValues,
        nullptr, // parameter lengths (can be null for text)
        nullptr, // parameter formats (can be null for text)
        0 // result format (0 for text)
    );

    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to get courses for student: " + error);
    }

    int rows = PQntuples(res);
    
    for(int i = 0; i < rows; ++i) {
        courseIds.push_back(std::stoi(PQgetvalue(res, i, 0)));
    }

    PQclear(res);
    return courseIds;
}

bool PostgresEnrollmentRepository::isEnrolled(int studentId, int courseId) {
    std::string studentIdStr = std::to_string(studentId);
    std::string courseIdStr = std::to_string(courseId);

    const char* paramValues[2] = {
        studentIdStr.c_str(),
        courseIdStr.c_str()
    };

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT 1 FROM enrollments WHERE student_id = $1 AND course_id = $2 LIMIT 1",
        2, // number of parameters
        nullptr, // parameter types (can be null for default)
        paramValues,
        nullptr, // parameter lengths (can be null for text)
        nullptr, // parameter formats (can be null for text)
        0 // result format (0 for text)
    );

    if(PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to check enrollment: " + error);
    }

    bool enrolled = PQntuples(res) > 0;
    
    PQclear(res);
    return enrolled;
}