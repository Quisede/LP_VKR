#pragma once

#include <QString>

struct TestData {
    int id = -1;
    int courseId = -1;
    QString title;
    QString status = "active";
    QString deadlineAt;
    bool available = true;
    int maxAttempts = 0;
    int timeLimitMinutes = 30;
    int attemptsUsed = 0;
    bool passed = false;
    double bestPercentage = 0.0;
    bool canAttempt = true;
};
