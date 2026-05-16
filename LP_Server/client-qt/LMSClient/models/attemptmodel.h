#pragma once

#include <QString>

struct AttemptData {
    int testId = -1;
    int courseId = -1;
    int score = 0;
    int total = 0;
    double percentage = 0.0;
    bool passed = false;
    QString testTitle;
    QString submittedAt;
};
