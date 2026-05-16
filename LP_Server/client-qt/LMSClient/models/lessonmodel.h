#pragma once

#include <QString>

struct LessonData {
    int id = -1;
    int courseId = -1;
    QString title;
    QString content;
    bool completed = false;
};
