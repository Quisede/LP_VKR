#pragma once

#include "TestRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Test.h"
#include <vector>

class PostgresTestRepository : public TestRepository {
    public:
        PostgresTestRepository(PostgresConnection& connection);

        std::vector<Test> getTestsForCourse(int courseId) override;
    private:
        PostgresConnection& db;
};