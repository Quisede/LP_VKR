#pragma once

#include <vector>
#include "../models/Question.h"
#include "../models/Submission.h"
#include "../models/TestResult.h"
#include "../models/Attempt.h"
#include "../repositories/QuestionRepository.h"
#include "../repositories/AttemptRepository.h"

class QuestionService {
public:
    QuestionService(QuestionRepository& qRepo,
                AttemptRepository& aRepo);

    std::vector<Question> getQuestionsForTest(int testId);

    TestResult checkAnswers(int testId,
                const std::vector<SubmittedAnswer>& answers);
    
    TestResult submitTest(int userId, int testId,
                const std::vector<SubmittedAnswer>& answers);

private:
    QuestionRepository& questionRepository;
    AttemptRepository& attemptRepository;
    int nextId = 1; // для генерации id попыток
};