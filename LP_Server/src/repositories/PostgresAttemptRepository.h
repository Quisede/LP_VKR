#pragma once

#include "AttemptRepository.h"
#include "../database/PostgresConnection.h"
#include <vector>

class PostgresAttemptRepository : public AttemptRepository {
    public:
        PostgresAttemptRepository(PostgresConnection& connection);

        void saveAttempt(const Attempt& attempt) override;
        std::vector<Attempt> getAttemptsForUser(int userId) override;
    private:
        PostgresConnection& connection;
};