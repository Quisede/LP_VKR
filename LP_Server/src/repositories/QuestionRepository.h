#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../models/Question.h"

class QuestionRepository {
public:
    virtual ~QuestionRepository() = default;

    virtual std::vector<Question> getQuestionsForTest(int testId) = 0;
    virtual std::optional<Question> getQuestionById(int questionId) = 0;
    virtual Question createQuestion(
        int testId,
        const std::string& text,
        const std::vector<std::string>& options,
        int correctOptionIndex) = 0;
    virtual Question updateQuestion(
        int questionId,
        const std::string& text,
        const std::vector<std::string>& options,
        int correctOptionIndex) = 0;
    virtual void deleteQuestion(int questionId) = 0;
};
