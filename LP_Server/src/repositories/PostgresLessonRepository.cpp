#include "PostgresLessonRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

PostgresLessonRepository::PostgresLessonRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Lesson> PostgresLessonRepository::getLessonsForCourse(int courseId) {
    std::lock_guard<std::mutex> lock(db.mutex());
    std::vector<Lesson> lessons;

    std::string query = "SELECT id, course_id, title, content FROM lessons WHERE course_id = " + std::to_string(courseId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get lessons for course: " + error);
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        Lesson lesson;

        lesson.id = std::stoi(PQgetvalue(res, i, 0));
        lesson.courseId = std::stoi(PQgetvalue(res, i, 1));
        lesson.title = PQgetvalue(res, i, 2);
        lesson.content = PQgetvalue(res, i, 3);
        
        lessons.push_back(lesson);
    }

    PQclear(res);
    return lessons;
}

std::optional<Lesson> PostgresLessonRepository::getLessonById(int lessonId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string lessonIdValue = std::to_string(lessonId);
    const char* params[] = {lessonIdValue.c_str()};

    PGresult* res = PQexecParams(
        db.get(),
        "SELECT id, course_id, title, content FROM lessons WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get lesson by id: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    Lesson lesson;
    lesson.id = std::stoi(PQgetvalue(res, 0, 0));
    lesson.courseId = std::stoi(PQgetvalue(res, 0, 1));
    lesson.title = PQgetvalue(res, 0, 2);
    lesson.content = PQgetvalue(res, 0, 3);
    PQclear(res);
    return lesson;
}

Lesson PostgresLessonRepository::createLesson(
    int courseId,
    const std::string& title,
    const std::string& content) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string courseIdValue = std::to_string(courseId);
    const char* params[] = {
        courseIdValue.c_str(),
        title.c_str(),
        content.c_str()
    };

    PGresult* res = PQexecParams(
        db.get(),
        "INSERT INTO lessons (course_id, title, content) "
        "VALUES ($1, $2, $3) "
        "RETURNING id, course_id, title, content",
        3,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to create lesson: " + error);
    }

    Lesson lesson;
    lesson.id = std::stoi(PQgetvalue(res, 0, 0));
    lesson.courseId = std::stoi(PQgetvalue(res, 0, 1));
    lesson.title = PQgetvalue(res, 0, 2);
    lesson.content = PQgetvalue(res, 0, 3);
    PQclear(res);
    return lesson;
}

Lesson PostgresLessonRepository::updateLesson(
    int lessonId,
    const std::string& title,
    const std::string& content) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string lessonIdValue = std::to_string(lessonId);
    const char* params[] = {
        lessonIdValue.c_str(),
        title.c_str(),
        content.c_str()
    };

    PGresult* res = PQexecParams(
        db.get(),
        "UPDATE lessons SET title = $2, content = $3 "
        "WHERE id = $1 "
        "RETURNING id, course_id, title, content",
        3,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to update lesson: " + error);
    }

    Lesson lesson;
    lesson.id = std::stoi(PQgetvalue(res, 0, 0));
    lesson.courseId = std::stoi(PQgetvalue(res, 0, 1));
    lesson.title = PQgetvalue(res, 0, 2);
    lesson.content = PQgetvalue(res, 0, 3);
    PQclear(res);
    return lesson;
}

void PostgresLessonRepository::deleteLesson(int lessonId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string lessonIdValue = std::to_string(lessonId);
    const char* params[] = {lessonIdValue.c_str()};

    PGresult* res = PQexecParams(
        db.get(),
        "DELETE FROM lessons WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to delete lesson: " + error);
    }

    PQclear(res);
}
