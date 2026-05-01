#include "LessonService.h"
#include <stdexcept>

LessonService::LessonService(LessonRepository& repo) 
    : lessonRepository(repo) {}

std::vector<Lesson> LessonService::getLessonsForCourse(int courseId) {
    return lessonRepository.getLessonsForCourse(courseId);
}

std::optional<Lesson> LessonService::getLessonById(int lessonId) {
    return lessonRepository.getLessonById(lessonId);
}

Lesson LessonService::createLesson(
    int courseId,
    const std::string& title,
    const std::string& content) {
    if (title.empty()) {
        throw std::invalid_argument("Lesson title must not be empty");
    }

    if (content.empty()) {
        throw std::invalid_argument("Lesson content must not be empty");
    }

    return lessonRepository.createLesson(courseId, title, content);
}
