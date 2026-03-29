#pragma once

#include <vector>
#include "../models/Test.h"

class TestRepository {
    public:
        virtual ~TestRepository() = default;

        virtual std::vector<Test> getTestsForCourse(int courseId) = 0;
};