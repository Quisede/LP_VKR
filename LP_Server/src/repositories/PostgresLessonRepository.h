#pragma once

#include "LessonRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Lesson.h"
#include <vector>

class PostgresLessonRepository : public LessonRepository {
public:
    PostgresLessonRepository(PostgresConnection& connection);

    std::vector<Lesson> getLessonsForCourse(int courseId) override;

private:
    PostgresConnection& db;
};
