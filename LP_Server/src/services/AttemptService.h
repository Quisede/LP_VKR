#pragma once

#include <vector>
#include "../models/Attempt.h"
#include "../repositories/AttemptRepository.h"

class AttemptService {
public:
    AttemptService(AttemptRepository& repo);

    std::vector<Attempt> getAttemptsForUser(int userId);

private:
    AttemptRepository& attemptRepository;
};