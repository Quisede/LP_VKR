#pragma once

#include <vector>
#include "../models/Attempt.h"
#include "../models/User.h"
#include "../repositories/AttemptRepository.h"

class AttemptService {
public:
    AttemptService(AttemptRepository& repo);

    std::vector<Attempt> getAttemptsForUser(int currentUserId, UserRole role, int requestedUserId);

private:
    AttemptRepository& attemptRepository;
};