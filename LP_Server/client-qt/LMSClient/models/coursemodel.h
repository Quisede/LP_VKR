#pragma once

#include <QString>

struct CourseData {
    int id = -1;
    QString title;
    QString description;
    int teacherId = -1;
    int lessonsCount = 0;
    int testsCount = 0;
    int studentsCount = 0;
};
