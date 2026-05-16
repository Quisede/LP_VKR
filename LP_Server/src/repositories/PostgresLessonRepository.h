#pragma once

#include "LessonRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Lesson.h"
#include <optional>
#include <vector>

class PostgresLessonRepository : public LessonRepository {
public:
    PostgresLessonRepository(PostgresConnection& connection);

    std::vector<Lesson> getLessonsForCourse(int courseId) override;
    std::vector<Lesson> getLessonsForCourseWithProgress(int courseId, int userId) override;
    std::optional<Lesson> getLessonById(int lessonId) override;
    void markLessonCompleted(int lessonId, int userId) override;
    Lesson createLesson(int courseId, const std::string& title, const std::string& content) override;
    Lesson updateLesson(int lessonId, const std::string& title, const std::string& content) override;
    void deleteLesson(int lessonId) override;

private:
    PostgresConnection& db;
};
