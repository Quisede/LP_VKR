#pragma once

#include <vector>
#include "../models/Lesson.h"

class LessonRepository {
public:
    virtual ~LessonRepository() = default;
    virtual std::vector<Lesson> getLessonsForCourse(int courseId) = 0;
};