#pragma once

#include <string>
#include <vector>

struct AnswerOption {
    int id;
    std::string text;
};

struct Question {
    int id;
    int testId;
    std::string text;

    std::vector<AnswerOption> options;

    int correctAnswerId; // скрыто для клиента
};