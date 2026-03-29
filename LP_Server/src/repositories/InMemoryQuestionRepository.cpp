#include "InMemoryQuestionRepository.h"

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