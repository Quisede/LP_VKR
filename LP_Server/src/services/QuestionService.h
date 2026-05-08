#pragma once

#include <optional>
#include <vector>
#include "../models/Question.h"
#include "../models/Submission.h"
#include "../models/TestResult.h"
#include "../models/Attempt.h"
#include "../models/User.h"
#include "../services/CourseService.h"
#include "../repositories/QuestionRepository.h"
#include "../repositories/AttemptRepository.h"
#include "../repositories/TestRepository.h"

class QuestionService {
public:
    QuestionService(QuestionRepository& qRepo,
                AttemptRepository& aRepo,
                TestRepository& tRepo,
                CourseService& cService);

    std::vector<Question> getQuestionsForTest(int testId);
    std::vector<Question> getQuestionsForManagement(int userId, UserRole role, int testId);
    std::optional<Question> getQuestionById(int questionId);
    Question createQuestion(
        int userId,
        UserRole role,
        int testId,
        const std::string& text,
        const std::vector<std::string>& options,
        int correctOptionIndex);
    Question updateQuestion(
        int userId,
        UserRole role,
        int questionId,
        const std::string& text,
        const std::vector<std::string>& options,
        int correctOptionIndex);
    void deleteQuestion(int userId, UserRole role, int questionId);

    TestResult checkAnswers(int testId,
                const std::vector<SubmittedAnswer>& answers);
    
    TestResult submitTest(int userId, int testId,
                const std::vector<SubmittedAnswer>& answers);

private:
    QuestionRepository& questionRepository;
    AttemptRepository& attemptRepository;
    TestRepository& testRepository;
    CourseService& courseService;
    int nextId = 1; // для генерации id попыток
};
