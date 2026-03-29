#pragma once

#include "QuestionRepository.h"
#include <vector>

class InMemoryQuestionRepository : public QuestionRepository {
    public:
        InMemoryQuestionRepository();

        std::vector<Question> getQuestionsForTest(int testId) override;
    private:
        std::vector<Question> questions;
};