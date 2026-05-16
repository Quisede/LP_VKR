#pragma once

#include <QString>

struct CourseStudentData {
    int id = -1;
    QString login;
    int progress = 0;
    int lessonProgress = 0;
    int testProgress = 0;
};
