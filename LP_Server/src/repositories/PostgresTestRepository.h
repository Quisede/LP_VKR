#pragma once

#include <optional>
#include "TestRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Test.h"
#include <vector>

class PostgresTestRepository : public TestRepository {
    public:
        PostgresTestRepository(PostgresConnection& connection);

        std::vector<Test> getTestsForCourse(int courseId) override;
        std::optional<Test> getTestById(int testId) override;
        Test createTest(int courseId, const std::string& title) override;
        Test updateTest(int testId, const std::string& title) override;
        void deleteTest(int testId) override;
    private:
        PostgresConnection& db;
};
