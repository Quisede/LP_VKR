#pragma once

#include "TestRepository.h"
#include <vector>

class InMemoryTestRepository : public TestRepository {
    public:
        InMemoryTestRepository();

        std::vector<Test> getTestsForCourse(int courseId) override;
    private:
        std::vector<Test> tests;
};