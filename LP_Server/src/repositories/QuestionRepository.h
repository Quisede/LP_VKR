#pragma once

#include <vector>
#include "../models/Question.h"

class QuestionRepository {
public:
    virtual ~QuestionRepository() = default;

    virtual std::vector<Question> getQuestionsForTest(int testId) = 0;
};