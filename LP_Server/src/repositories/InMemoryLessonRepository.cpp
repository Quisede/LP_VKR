#include "InMemoryLessonRepository.h"
#include <algorithm>
#include <stdexcept>

InMemoryLessonRepository::InMemoryLessonRepository() {
    lessons.push_back({1, 1, "Introduction", "Welcome to the course"});
    lessons.push_back({2, 1, "Variables", "C++ variables explanation"});
    lessons.push_back({3, 2, "OOP Basics", "Intro to OOP"});
}

std::vector<Lesson> InMemoryLessonRepository::getLessonsForCourse(int courseId) {
    std::vector<Lesson> result;

    for(const auto& lesson : lessons) {
        if(lesson.courseId == courseId) {
            result.push_back(lesson);
        }
    }

    return result;
}

std::optional<Lesson> InMemoryLessonRepository::getLessonById(int lessonId) {
    for (const auto& lesson : lessons) {
        if (lesson.id == lessonId) {
            return lesson;
        }
    }

    return std::nullopt;
}

Lesson InMemoryLessonRepository::createLesson(
    int courseId,
    const std::string& title,
    const std::string& content) {
    int nextId = lessons.empty() ? 1 : lessons.back().id + 1;
    Lesson lesson{nextId, courseId, title, content};
    lessons.push_back(lesson);
    return lesson;
}

Lesson InMemoryLessonRepository::updateLesson(
    int lessonId,
    const std::string& title,
    const std::string& content) {
    for (auto &lesson : lessons) {
        if (lesson.id == lessonId) {
            lesson.title = title;
            lesson.content = content;
            return lesson;
        }
    }

    throw std::runtime_error("Lesson not found");
}

void InMemoryLessonRepository::deleteLesson(int lessonId) {
    lessons.erase(
        std::remove_if(
            lessons.begin(),
            lessons.end(),
            [lessonId](const Lesson &lesson) { return lesson.id == lessonId; }),
        lessons.end());
}
