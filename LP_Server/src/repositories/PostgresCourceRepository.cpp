#include "PostgresCourseRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

PostgresCourseRepository::PostgresCourseRepository(
    PostgresConnection& conn)
    : connection(conn) {}

std::vector<Course> PostgresCourseRepository::getCoursesForStudent(int userId) {
    std::lock_guard<std::mutex> lock(connection.mutex());
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
    std::lock_guard<std::mutex> lock(connection.mutex());

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
    std::lock_guard<std::mutex> lock(connection.mutex());
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

std::optional<Course> PostgresCourseRepository::getCourseById(int courseId) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string courseIdValue = std::to_string(courseId);
    const char* params[] = {courseIdValue.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "SELECT id, title, description, teacher_id FROM courses WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to get course by id: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    Course course;
    course.id = std::stoi(PQgetvalue(res, 0, 0));
    course.title = PQgetvalue(res, 0, 1);
    course.description = PQgetvalue(res, 0, 2);
    course.teacherId = std::stoi(PQgetvalue(res, 0, 3));
    PQclear(res);
    return course;
}

std::vector<Course> PostgresCourseRepository::getAllCourses() {
    std::lock_guard<std::mutex> lock(connection.mutex());
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

Course PostgresCourseRepository::createCourse(
    const std::string& title,
    const std::string& description,
    int teacherId) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string teacherIdValue = std::to_string(teacherId);
    const char* params[] = {
        title.c_str(),
        description.c_str(),
        teacherIdValue.c_str()
    };

    PGresult* res = PQexecParams(
        connection.get(),
        "INSERT INTO courses (title, description, teacher_id) "
        "VALUES ($1, $2, $3) "
        "RETURNING id, title, description, teacher_id",
        3,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to create course: " + error);
    }

    Course course;
    course.id = std::stoi(PQgetvalue(res, 0, 0));
    course.title = PQgetvalue(res, 0, 1);
    course.description = PQgetvalue(res, 0, 2);
    course.teacherId = std::stoi(PQgetvalue(res, 0, 3));

    PQclear(res);
    return course;
}

Course PostgresCourseRepository::updateCourse(
    int courseId,
    const std::string& title,
    const std::string& description) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string courseIdValue = std::to_string(courseId);
    const char* params[] = {
        title.c_str(),
        description.c_str(),
        courseIdValue.c_str()
    };

    PGresult* res = PQexecParams(
        connection.get(),
        "UPDATE courses "
        "SET title = $1, description = $2 "
        "WHERE id = $3 "
        "RETURNING id, title, description, teacher_id",
        3,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to update course: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        throw std::runtime_error("Course not found");
    }

    Course course;
    course.id = std::stoi(PQgetvalue(res, 0, 0));
    course.title = PQgetvalue(res, 0, 1);
    course.description = PQgetvalue(res, 0, 2);
    course.teacherId = std::stoi(PQgetvalue(res, 0, 3));

    PQclear(res);
    return course;
}

void PostgresCourseRepository::deleteCourse(int courseId) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::string courseIdValue = std::to_string(courseId);
    const char* params[] = {courseIdValue.c_str()};

    PGresult* res = PQexecParams(
        connection.get(),
        "DELETE FROM courses WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to delete course: " + error);
    }

    PQclear(res);
}

std::vector<Course> PostgresCourseRepository::getCoursesPaged(int userId, const std::string& role, int limit, int offset) {
    std::lock_guard<std::mutex> lock(connection.mutex());

    std::vector<Course> courses;

    std::string query;

    if (role == "Student") {
        query =
            "SELECT c.id, c.title, c.description, c.teacher_id "
            "FROM courses c "
            "JOIN enrollments e ON c.id = e.course_id "
            "WHERE e.student_id = " + std::to_string(userId) + " "
            "ORDER BY c.id "
            "LIMIT " + std::to_string(limit) +
            " OFFSET " + std::to_string(offset);
    } else if (role == "Teacher") {
        query =
            "SELECT id, title, description, teacher_id "
            "FROM courses "
            "WHERE teacher_id = " + std::to_string(userId) + " "
            "ORDER BY id "
            "LIMIT " + std::to_string(limit) +
            " OFFSET " + std::to_string(offset);
    } else {
        query =
            "SELECT id, title, description, teacher_id "
            "FROM courses "
            "ORDER BY id "
            "LIMIT " + std::to_string(limit) +
            " OFFSET " + std::to_string(offset);
    }

    PGresult* res = PQexec(connection.get(), query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(connection.get());
        PQclear(res);
        throw std::runtime_error("Failed to get paged courses: " + error);
    }

    int rows = PQntuples(res);

    for (int i = 0; i < rows; i++) {
        Course c;

        c.id = std::stoi(PQgetvalue(res, i, 0));
        c.title = PQgetvalue(res, i, 1);
        c.description = PQgetvalue(res, i, 2);
        c.teacherId = std::stoi(PQgetvalue(res, i, 3));

        courses.push_back(c);
    }

    PQclear(res);
    return courses;
}
