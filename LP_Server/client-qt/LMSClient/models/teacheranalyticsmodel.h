#pragma once

#include <QString>
#include <QVector>

struct TeacherAnalyticsRowData {
    QString studentLogin;
    QString testTitle;
    int score = 0;
    int total = 0;
    double percentage = 0.0;
    bool passed = false;
};

struct TeacherCourseAnalyticsData {
    int studentsCount = 0;
    int attemptsCount = 0;
    double averagePercentage = 0.0;
    double averageLessonProgress = 0.0;
    QVector<TeacherAnalyticsRowData> rows;
};
