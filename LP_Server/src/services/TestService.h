#pragma once

#include <vector>
#include "../models/Test.h"
#include "../models/User.h"
#include "../repositories/TestRepository.h"
#include "../repositories/EnrollmentRepository.h"

class TestService {
public:
    TestService(TestRepository& testRepo,
            EnrollmentRepository& enrollRepo);

    std::vector<Test> getTestsForCourse(int userId, UserRole role, int courseId);

private:
    TestRepository& testRepository;
    EnrollmentRepository& enrollmentRepository;
};