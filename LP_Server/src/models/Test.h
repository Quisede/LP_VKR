#pragma once

#include <string>

struct Test {
    int id; // идентификатор
    int courseId; // айди курса к которому привязан тест
    std::string title; // название
    std::string status = "active";
    std::string deadlineAt;
    bool available = true;
    int maxAttempts = 0;
    int timeLimitMinutes = 30;
    int attemptsUsed = 0;
    bool passed = false;
    double bestPercentage = 0.0;
    bool canAttempt = true;
};
