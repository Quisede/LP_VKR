#include "LessonService.h"
#include <stdexcept>

LessonService::LessonService(LessonRepository& repo) 
    : lessonRepository(repo) {}

std::vector<Lesson> LessonService::getLessonsForCourse(int courseId) {
    return lessonRepository.getLessonsForCourse(courseId);
}

std::vector<Lesson> LessonService::getLessonsForCourseForUser(int courseId, int userId) {
    if (userId <= 0) {
        return lessonRepository.getLessonsForCourse(courseId);
    }

    return lessonRepository.getLessonsForCourseWithProgress(courseId, userId);
}

std::optional<Lesson> LessonService::getLessonById(int lessonId) {
    return lessonRepository.getLessonById(lessonId);
}

void LessonService::markLessonCompleted(int lessonId, int userId) {
    if (lessonId <= 0 || userId <= 0) {
        throw std::invalid_argument("Lesson and user must be valid");
    }

    lessonRepository.markLessonCompleted(lessonId, userId);
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

Lesson LessonService::updateLesson(
    int lessonId,
    const std::string& title,
    const std::string& content) {
    if (title.empty()) {
        throw std::invalid_argument("Lesson title must not be empty");
    }

    if (content.empty()) {
        throw std::invalid_argument("Lesson content must not be empty");
    }

    return lessonRepository.updateLesson(lessonId, title, content);
}

void LessonService::deleteLesson(int lessonId) {
    lessonRepository.deleteLesson(lessonId);
}
