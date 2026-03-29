#pragma once

#include <vector>
#include "../models/Question.h"
#include "../repositories/QuestionRepository.h"

class QuestionService {
public:
    QuestionService(QuestionRepository& repo);

    std::vector<Question> getQuestionsForTest(int testId);

private:
    QuestionRepository& questionRepository;
};