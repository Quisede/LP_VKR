#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../models/Lesson.h"
#include "../repositories/LessonRepository.h"

class LessonService {
public:
    LessonService(LessonRepository& repo);

    std::vector<Lesson> getLessonsForCourse(int courseId);
    std::optional<Lesson> getLessonById(int lessonId);
    Lesson createLesson(int courseId, const std::string& title, const std::string& content);
    Lesson updateLesson(int lessonId, const std::string& title, const std::string& content);
    void deleteLesson(int lessonId);

private:
    LessonRepository& lessonRepository;
};
