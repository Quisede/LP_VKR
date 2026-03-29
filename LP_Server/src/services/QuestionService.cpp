#include "QuestionService.h"

QuestionService::QuestionService(QuestionRepository& qRepo,
                AttemptRepository& aRepo)
    : questionRepository(qRepo),
      attemptRepository(aRepo) {}

std::vector<Question> QuestionService::getQuestionsForTest(int testId) {
    return questionRepository.getQuestionsForTest(testId);
}

TestResult QuestionService::checkAnswers(
    int testId,
    const std::vector<SubmittedAnswer>& answers) {

    auto questions = questionRepository.getQuestionsForTest(testId);

    int score = 0;
    int total = questions.size();

    for (const auto& q : questions) {
        for (const auto& a : answers) {
            if (a.questionId == q.id &&
                a.answerId == q.correctAnswerId) {
                score++;
            }
        }
    }

    double percentage = total == 0 ? 0.0 : (double)score / total * 100.0;

    bool passed = percentage >= 60.0; // порог

    return {score, total, percentage, passed};
}

TestResult QuestionService::submitTest(
    int userId,
    int testId,
    const std::vector<SubmittedAnswer>& answers) {

    auto questions = questionRepository.getQuestionsForTest(testId);

    int score = 0;
    int total = questions.size();

    for (const auto& q : questions) {
        for (const auto& a : answers) {
            if (a.questionId == q.id &&
                a.answerId == q.correctAnswerId) {
                score++;
            }
        }
    }

    double percentage = total == 0 ? 0.0 : (double)score / total * 100.0;
    bool passed = percentage >= 60.0;

    TestResult result {score, total, percentage, passed};

    // 🔥 ВОТ ЗДЕСЬ вставляется твой код
    Attempt attempt {
        nextId++,
        userId,
        testId,
        result.score,
        result.total,
        result.percentage,
        result.passed
    };

    attemptRepository.saveAttempt(attempt);

    return result;
}