#include "InMemoryLessonRepository.h"

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