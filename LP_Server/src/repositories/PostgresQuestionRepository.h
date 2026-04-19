#pragma once

#include "QuestionRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Question.h"
#include <vector>

class PostgresQuestionRepository : public QuestionRepository {
    public:
        PostgresQuestionRepository(PostgresConnection& connection);

        std::vector<Question> getQuestionsForTest(int testId) override;
    private:
        PostgresConnection& db;
};