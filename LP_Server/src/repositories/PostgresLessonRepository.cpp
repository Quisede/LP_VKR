#include "PostgresLessonRepository.h"
#include <libpq-fe.h>

PostgresLessonRepository::PostgresLessonRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Lesson> PostgresLessonRepository::getLessonsForCourse(int courseId) {
    std::vector<Lesson> lessons;

    std::string query = "SELECT id, course_id, title, content FROM lessons WHERE course_id = " + std::to_string(courseId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());

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