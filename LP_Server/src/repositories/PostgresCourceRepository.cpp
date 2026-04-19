#include "PostgresCourseRepository.h"
#include <libpq-fe.h>

PostgresCourseRepository::PostgresCourseRepository(
    PostgresConnection& conn)
    : connection(conn) {}

std::vector<Course> PostgresCourseRepository::getCoursesForStudent(int userId) {
    std::vector<Course> courses;

    std::string query = 
        "SELECT c.id, c.title, c.description, c.teacher_id "
        "FROM courses c "
        "JOIN enrollments e ON c.id = e.course_id "
        "WHERE e.student_id = " + std::to_string(userId);
    
    PGresult* res = PQexec(connection.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return courses;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        Course course;
        course.id = std::stoi(PQgetvalue(res, i, 0));
        course.title = PQgetvalue(res, i, 1);
        course.description = PQgetvalue(res, i, 2);
        course.teacherId = std::stoi(PQgetvalue(res, i, 3));
        courses.push_back(course);
    }

    PQclear(res);
    return courses;
}

std::vector<Course> PostgresCourseRepository::getCoursesForTeacher(int teacherId) {

    std::vector<Course> courses;

    std::string query =
        "SELECT id,title,description,teacher_id "
        "FROM courses WHERE teacher_id=" +
        std::to_string(teacherId);

    PGresult* res = PQexec(connection.get(), query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return courses;
    }

    int rows = PQntuples(res);

    for(int i=0;i<rows;i++) {

        Course c;

        c.id = std::stoi(PQgetvalue(res,i,0));
        c.title = PQgetvalue(res,i,1);
        c.description = PQgetvalue(res,i,2);
        c.teacherId = std::stoi(PQgetvalue(res,i,3));

        courses.push_back(c);
    }

    PQclear(res);

    return courses;
}

std::vector<Course> PostgresCourseRepository::getCoursesByIds(
    const std::vector<int>& courseIds) {
    std::vector<Course> courses;

    if (courseIds.empty()) {
        return courses;
    }

    std::string query =
        "SELECT id, title, description, teacher_id "
        "FROM courses WHERE id IN (";

    for (size_t i = 0; i < courseIds.size(); ++i) {
        if (i > 0) {
            query += ",";
        }
        query += std::to_string(courseIds[i]);
    }

    query += ")";

    PGresult* res = PQexec(connection.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return courses;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        Course course;
        course.id = std::stoi(PQgetvalue(res, i, 0));
        course.title = PQgetvalue(res, i, 1);
        course.description = PQgetvalue(res, i, 2);
        course.teacherId = std::stoi(PQgetvalue(res, i, 3));
        courses.push_back(course);
    }

    PQclear(res);
    return courses;
}

std::vector<Course> PostgresCourseRepository::getAllCourses() {
    std::vector<Course> courses;

    std::string query =
        "SELECT id, title, description, teacher_id "
        "FROM courses";

    PGresult* res = PQexec(connection.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        PQclear(res);
        return courses;
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        Course course;
        course.id = std::stoi(PQgetvalue(res, i, 0));
        course.title = PQgetvalue(res, i, 1);
        course.description = PQgetvalue(res, i, 2);
        course.teacherId = std::stoi(PQgetvalue(res, i, 3));
        courses.push_back(course);
    }

    PQclear(res);
    return courses;
}
