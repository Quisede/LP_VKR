#include "InMemoryQuestionRepository.h"
#include <algorithm>
#include <optional>
#include <stdexcept>

InMemoryQuestionRepository::InMemoryQuestionRepository() {

    questions.push_back({
        1,
        1,
        "2 + 2 = ?",
        {
            {1, "3"},
            {2, "4"},
            {3, "5"}
        },
        2 // правильный ответ
    });

    questions.push_back({
        2,
        1,
        "C++ это?",
        {
            {4, "Язык программирования"},
            {5, "База данных"},
            {6, "ОС"}
        },
        4
    });
}

std::vector<Question> InMemoryQuestionRepository::getQuestionsForTest(int testId) {

    std::vector<Question> result;

    for (const auto& q : questions) {
        if (q.testId == testId) {
            result.push_back(q);
        }
    }

    return result;
}

std::optional<Question> InMemoryQuestionRepository::getQuestionById(int questionId) {
    for (const auto& question : questions) {
        if (question.id == questionId) {
            return question;
        }
    }

    return std::nullopt;
}

Question InMemoryQuestionRepository::createQuestion(
    int testId,
    const std::string& text,
    const std::vector<std::string>& options,
    int correctOptionIndex) {
    const int nextQuestionId = questions.empty() ? 1 : questions.back().id + 1;

    int nextAnswerId = 1;
    for (const auto& question : questions) {
        for (const auto& option : question.options) {
            if (option.id >= nextAnswerId) {
                nextAnswerId = option.id + 1;
            }
        }
    }

    Question question;
    question.id = nextQuestionId;
    question.testId = testId;
    question.text = text;

    for (const auto& optionText : options) {
        question.options.push_back({nextAnswerId++, optionText});
    }

    question.correctAnswerId = question.options[correctOptionIndex].id;
    questions.push_back(question);
    return question;
}

Question InMemoryQuestionRepository::updateQuestion(
    int questionId,
    const std::string& text,
    const std::vector<std::string>& options,
    int correctOptionIndex) {
    int nextAnswerId = 1;
    for (const auto& question : questions) {
        for (const auto& option : question.options) {
            if (option.id >= nextAnswerId) {
                nextAnswerId = option.id + 1;
            }
        }
    }

    for (auto& question : questions) {
        if (question.id == questionId) {
            question.text = text;
            question.options.clear();

            for (const auto& optionText : options) {
                question.options.push_back({nextAnswerId++, optionText});
            }

            question.correctAnswerId = question.options[correctOptionIndex].id;
            return question;
        }
    }

    throw std::runtime_error("Question not found");
}

void InMemoryQuestionRepository::deleteQuestion(int questionId) {
    questions.erase(
        std::remove_if(
            questions.begin(),
            questions.end(),
            [questionId](const Question& question) { return question.id == questionId; }),
        questions.end());
}
