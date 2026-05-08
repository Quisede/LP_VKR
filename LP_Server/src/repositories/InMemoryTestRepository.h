#pragma once

#include <optional>
#include "TestRepository.h"
#include <vector>

class InMemoryTestRepository : public TestRepository {
    public:
        InMemoryTestRepository();

        std::vector<Test> getTestsForCourse(int courseId) override;
        std::optional<Test> getTestById(int testId) override;
        Test createTest(int courseId, const std::string& title) override;
        Test updateTest(int testId, const std::string& title) override;
        void deleteTest(int testId) override;
    private:
        std::vector<Test> tests;
};
