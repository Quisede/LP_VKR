#pragma once

#include <QString>

struct TeacherStudentAttemptData {
    QString testTitle;
    int score = 0;
    int total = 0;
    double percentage = 0.0;
    bool passed = false;
};
