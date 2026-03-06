#include "LessonService.h"

LessonService::LessonService(LessonRepository& repo) 
    : lessonRepository(repo) {}

std::vector<Lesson> LessonService::getLessonsForCourse(int courseId) {
    return lessonRepository.getLessonsForCourse(courseId);
}