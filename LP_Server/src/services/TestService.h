#pragma once

#include <optional>
#include <string>
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
    Test createTest(int courseId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes);
    Test updateTest(int testId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes);
    void deleteTest(int testId);
    std::optional<Test> getTestById(int testId);

private:
    TestRepository& testRepository;
    EnrollmentRepository& enrollmentRepository;
};
