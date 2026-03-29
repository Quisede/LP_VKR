#pragma once

#include <vector>
#include "../models/Attempt.h"

class AttemptRepository {
public:
    virtual ~AttemptRepository() = default;

    virtual void saveAttempt(const Attempt& attempt) = 0;
    virtual std::vector<Attempt> getAttemptsForUser(int userId) = 0;
};