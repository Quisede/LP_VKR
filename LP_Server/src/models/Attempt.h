#pragma once
// сохранение результата попытки прохождения теста пользователем
#include <string>

struct Attempt {
    int id;
    int userId;
    int testId;
    int score;
    int total;
    double percentage;
    bool passed;
};