#pragma once

#include <string>
#include <vector>
#include "../models/Test.h"

class TestRepository {
    public:
        virtual ~TestRepository() = default;

        virtual std::vector<Test> getTestsForCourse(int courseId) = 0;
        virtual Test createTest(int courseId, const std::string& title) = 0;
};
