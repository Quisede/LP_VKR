#pragma once

#include <vector>
#include "../models/Lesson.h"
#include "../repositories/LessonRepository.h"

class LessonService {
public:
    LessonService(LessonRepository& repo);

    std::vector<Lesson> getLessonsForCourse(int courseId);

private:
    LessonRepository& lessonRepository;
};