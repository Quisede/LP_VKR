#pragma once

#include "LessonRepository.h"
#include <set>
#include <vector>

class InMemoryLessonRepository : public LessonRepository {
public:
    InMemoryLessonRepository();

    std::vector<Lesson> getLessonsForCourse(int courseId) override;
    std::vector<Lesson> getLessonsForCourseWithProgress(int courseId, int userId) override;
    std::optional<Lesson> getLessonById(int lessonId) override;
    void markLessonCompleted(int lessonId, int userId) override;
    Lesson createLesson(int courseId, const std::string& title, const std::string& content) override;
    Lesson updateLesson(int lessonId, const std::string& title, const std::string& content) override;
    void deleteLesson(int lessonId) override;
private:
    std::vector<Lesson> lessons;
    std::set<std::pair<int, int>> completedLessons;
};
