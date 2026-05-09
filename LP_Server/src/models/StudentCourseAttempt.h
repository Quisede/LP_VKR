#pragma once

#include <string>

struct StudentCourseAttempt {
    std::string testTitle;
    int score = 0;
    int total = 0;
    double percentage = 0.0;
    bool passed = false;
};
