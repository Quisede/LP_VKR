#include "QuestionService.h"

QuestionService::QuestionService(QuestionRepository& repo)
    : questionRepository(repo) {}

std::vector<Question> QuestionService::getQuestionsForTest(int testId) {
    return questionRepository.getQuestionsForTest(testId);
}
