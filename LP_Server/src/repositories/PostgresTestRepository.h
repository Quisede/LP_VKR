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
        std::vector<Test> getTestsForCourseForStudent(int courseId, int userId) override;
        std::optional<Test> getTestById(int testId) override;
        Test createTest(int courseId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) override;
        Test updateTest(int testId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) override;
        void deleteTest(int testId) override;
    private:
        void ensureSchema();
        PostgresConnection& db;
};
