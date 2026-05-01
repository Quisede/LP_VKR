#pragma once

#include "TestRepository.h"
#include <vector>

class InMemoryTestRepository : public TestRepository {
    public:
        InMemoryTestRepository();

        std::vector<Test> getTestsForCourse(int courseId) override;
        Test createTest(int courseId, const std::string& title) override;
    private:
        std::vector<Test> tests;
};
