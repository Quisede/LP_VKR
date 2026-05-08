#pragma once

#include "QuestionRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Question.h"
#include <optional>
#include <vector>

class PostgresQuestionRepository : public QuestionRepository {
    public:
        PostgresQuestionRepository(PostgresConnection& connection);

        std::vector<Question> getQuestionsForTest(int testId) override;
        std::optional<Question> getQuestionById(int questionId) override;
        Question createQuestion(
            int testId,
            const std::string& text,
            const std::vector<std::string>& options,
            int correctOptionIndex) override;
        Question updateQuestion(
            int questionId,
            const std::string& text,
            const std::vector<std::string>& options,
            int correctOptionIndex) override;
        void deleteQuestion(int questionId) override;
    private:
        PostgresConnection& db;
};
