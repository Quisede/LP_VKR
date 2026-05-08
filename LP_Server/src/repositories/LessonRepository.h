#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../models/Lesson.h"

class LessonRepository {
public:
    virtual ~LessonRepository() = default;
    virtual std::vector<Lesson> getLessonsForCourse(int courseId) = 0;
    virtual std::optional<Lesson> getLessonById(int lessonId) = 0;
    virtual Lesson createLesson(
        int courseId,
        const std::string& title,
        const std::string& content) = 0;
    virtual Lesson updateLesson(
        int lessonId,
        const std::string& title,
        const std::string& content) = 0;
    virtual void deleteLesson(int lessonId) = 0;
};
