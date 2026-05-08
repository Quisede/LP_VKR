#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../models/Test.h"

class TestRepository {
    public:
        virtual ~TestRepository() = default;

        virtual std::vector<Test> getTestsForCourse(int courseId) = 0;
        virtual std::optional<Test> getTestById(int testId) = 0;
        virtual Test createTest(int courseId, const std::string& title) = 0;
        virtual Test updateTest(int testId, const std::string& title) = 0;
        virtual void deleteTest(int testId) = 0;
};
