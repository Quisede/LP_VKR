#pragma once

#include <optional>
#include "TestRepository.h"
#include <vector>

class InMemoryTestRepository : public TestRepository {
    public:
        InMemoryTestRepository();

        std::vector<Test> getTestsForCourse(int courseId) override;
        std::vector<Test> getTestsForCourseForStudent(int courseId, int userId) override;
        std::optional<Test> getTestById(int testId) override;
        Test createTest(int courseId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) override;
        Test updateTest(int testId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) override;
        void deleteTest(int testId) override;
    private:
        std::vector<Test> tests;
};
