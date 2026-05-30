#pragma once

#include <string>

struct CourseStudent {
    int id;
    std::string login;
    std::string groupName;
    int progress;
    int lessonProgress = 0;
    int testProgress = 0;
};
