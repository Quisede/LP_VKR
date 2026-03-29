#include "AttemptService.h"

AttemptService::AttemptService(AttemptRepository& repo)
    : attemptRepository(repo) {}

std::vector<Attempt> AttemptService::getAttemptsForUser(int userId) {
    return attemptRepository.getAttemptsForUser(userId);
}