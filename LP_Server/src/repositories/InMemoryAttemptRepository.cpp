#include "InMemoryAttemptRepository.h"

InMemoryAttemptRepository::InMemoryAttemptRepository() {
    // можно добавить начальные данные, если нужно
    attempts.push_back(Attempt{1, 1, 1, 85, 100, 85.0, true});
    attempts.push_back(Attempt{2, 1, 1, 90, 100, 90.0, true});
    attempts.push_back(Attempt{3, 2, 1, 75, 100, 75.0, true});
}   

void InMemoryAttemptRepository::saveAttempt(const Attempt& attempt) {
    attempts.push_back(attempt);
}

std::vector<Attempt> InMemoryAttemptRepository::getAttemptsForUser(int userId) {

    std::vector<Attempt> result;

    for (const auto& a : attempts) {
        if (a.userId == userId) {
            result.push_back(a);
        }
    }

    return result;
}