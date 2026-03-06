#pragma once

#include "LessonRepository.h"
#include <vector>

class InMemoryLessonRepository : public LessonRepository {
public:
    InMemoryLessonRepository();

    std::vector<Lesson> getLessonsForCourse(int courseId) override;
private:
    std::vector<Lesson> lessons;
};