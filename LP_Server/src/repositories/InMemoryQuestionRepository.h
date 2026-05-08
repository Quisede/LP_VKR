#pragma once

#include "QuestionRepository.h"
#include <vector>

class InMemoryQuestionRepository : public QuestionRepository {
    public:
        InMemoryQuestionRepository();

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
        std::vector<Question> questions;
};
