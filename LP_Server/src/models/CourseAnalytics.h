#pragma once

#include <string>
#include <vector>

struct CourseAnalyticsRow {
    std::string studentLogin;
    std::string testTitle;
    int score;
    int total;
    double percentage;
    bool passed;
};

struct CourseAnalytics {
    int studentsCount = 0;
    int attemptsCount = 0;
    double averagePercentage = 0.0;
    std::vector<CourseAnalyticsRow> rows;
};
