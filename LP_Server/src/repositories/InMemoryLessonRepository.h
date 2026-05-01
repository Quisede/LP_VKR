#pragma once

#include "LessonRepository.h"
#include <vector>

class InMemoryLessonRepository : public LessonRepository {
public:
    InMemoryLessonRepository();

    std::vector<Lesson> getLessonsForCourse(int courseId) override;
    std::optional<Lesson> getLessonById(int lessonId) override;
    Lesson createLesson(int courseId, const std::string& title, const std::string& content) override;
private:
    std::vector<Lesson> lessons;
};
