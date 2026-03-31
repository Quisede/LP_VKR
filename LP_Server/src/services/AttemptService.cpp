#include "AttemptService.h"

AttemptService::AttemptService(AttemptRepository& repo)
    : attemptRepository(repo) {}

std::vector<Attempt> AttemptService::getAttemptsForUser(
    int currentUserId,
    UserRole role,
    int requestedUserId) {

    // проверка доступа
    if (currentUserId != requestedUserId &&
        role != UserRole::Admin) {

        return {}; // можно позже сделать exception
    }

    return attemptRepository.getAttemptsForUser(requestedUserId);
}