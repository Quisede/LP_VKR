#include "QuestionService.h"

#include <algorithm>
#include <stdexcept>

namespace {

std::string trimCopy(const std::string& value) {
    const auto begin = value.find_first_not_of(" \t\n\r");
    if (begin == std::string::npos) {
        return "";
    }

    const auto end = value.find_last_not_of(" \t\n\r");
    return value.substr(begin, end - begin + 1);
}

}

QuestionService::QuestionService(QuestionRepository& qRepo,
                AttemptRepository& aRepo,
                TestRepository& tRepo,
                CourseService& cService)
    : questionRepository(qRepo),
      attemptRepository(aRepo),
      testRepository(tRepo),
      courseService(cService) {}

std::vector<Question> QuestionService::getQuestionsForTest(int testId) {
    return questionRepository.getQuestionsForTest(testId);
}

std::vector<Question> QuestionService::getQuestionsForManagement(int userId, UserRole role, int testId) {
    auto test = testRepository.getTestById(testId);
    if (!test.has_value()) {
        throw std::invalid_argument("Test not found");
    }

    if (!courseService.canManageCourse(userId, role, test->courseId)) {
        throw std::invalid_argument("You can manage only tests of your own courses");
    }

    return questionRepository.getQuestionsForTest(testId);
}

std::optional<Question> QuestionService::getQuestionById(int questionId) {
    return questionRepository.getQuestionById(questionId);
}

Question QuestionService::createQuestion(
    int userId,
    UserRole role,
    int testId,
    const std::string& text,
    const std::vector<std::string>& options,
    int correctOptionIndex) {
    auto test = testRepository.getTestById(testId);
    if (!test.has_value()) {
        throw std::invalid_argument("Test not found");
    }

    if (!courseService.canManageCourse(userId, role, test->courseId)) {
        throw std::invalid_argument("You can manage only tests of your own courses");
    }

    const std::string normalizedText = trimCopy(text);
    if (normalizedText.empty()) {
        throw std::invalid_argument("Question text must not be empty");
    }

    if (options.size() < 2) {
        throw std::invalid_argument("Question must contain at least two answer options");
    }

    std::vector<std::string> normalizedOptions;
    normalizedOptions.reserve(options.size());

    for (const auto& option : options) {
        std::string normalizedOption = trimCopy(option);
        if (normalizedOption.empty()) {
            throw std::invalid_argument("Answer option text must not be empty");
        }

        normalizedOptions.push_back(normalizedOption);
    }

    if (correctOptionIndex < 0 || correctOptionIndex >= static_cast<int>(normalizedOptions.size())) {
        throw std::invalid_argument("Correct option index is out of range");
    }

    return questionRepository.createQuestion(
        testId,
        normalizedText,
        normalizedOptions,
        correctOptionIndex);
}

Question QuestionService::updateQuestion(
    int userId,
    UserRole role,
    int questionId,
    const std::string& text,
    const std::vector<std::string>& options,
    int correctOptionIndex) {
    auto question = questionRepository.getQuestionById(questionId);
    if (!question.has_value()) {
        throw std::invalid_argument("Question not found");
    }

    auto test = testRepository.getTestById(question->testId);
    if (!test.has_value()) {
        throw std::invalid_argument("Test not found");
    }

    if (!courseService.canManageCourse(userId, role, test->courseId)) {
        throw std::invalid_argument("You can manage only tests of your own courses");
    }

    const std::string normalizedText = trimCopy(text);
    if (normalizedText.empty()) {
        throw std::invalid_argument("Question text must not be empty");
    }

    if (options.size() < 2) {
        throw std::invalid_argument("Question must contain at least two answer options");
    }

    std::vector<std::string> normalizedOptions;
    normalizedOptions.reserve(options.size());

    for (const auto& option : options) {
        std::string normalizedOption = trimCopy(option);
        if (normalizedOption.empty()) {
            throw std::invalid_argument("Answer option text must not be empty");
        }

        normalizedOptions.push_back(normalizedOption);
    }

    if (correctOptionIndex < 0 || correctOptionIndex >= static_cast<int>(normalizedOptions.size())) {
        throw std::invalid_argument("Correct option index is out of range");
    }

    return questionRepository.updateQuestion(
        questionId,
        normalizedText,
        normalizedOptions,
        correctOptionIndex);
}

void QuestionService::deleteQuestion(int userId, UserRole role, int questionId) {
    auto question = questionRepository.getQuestionById(questionId);
    if (!question.has_value()) {
        throw std::invalid_argument("Question not found");
    }

    auto test = testRepository.getTestById(question->testId);
    if (!test.has_value()) {
        throw std::invalid_argument("Test not found");
    }

    if (!courseService.canManageCourse(userId, role, test->courseId)) {
        throw std::invalid_argument("You can manage only tests of your own courses");
    }

    questionRepository.deleteQuestion(questionId);
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

    auto test = testRepository.getTestById(testId);
    TestResult result {
        score,
        total,
        percentage,
        passed,
        test.has_value() ? test->courseId : -1
    };

    Attempt attempt {
        nextId++,
        userId,
        testId,
        result.courseId,
        result.score,
        result.total,
        result.percentage,
        result.passed,
        test.has_value() ? test->title : "",
        ""
    };

    attemptRepository.saveAttempt(attempt);

    return result;
}
