#include "TestService.h"
#include <stdexcept>

namespace {
std::string normalizedStatus(const std::string& value) {
    if (value.empty()) {
        return "active";
    }

    if (value != "active" && value != "closed") {
        throw std::invalid_argument("Test status must be active or closed");
    }

    return value;
}
}

TestService::TestService(TestRepository& testRepo,
            EnrollmentRepository& enrollRepo)
    : testRepository(testRepo), enrollmentRepository(enrollRepo) {}

std::vector<Test> TestService::getTestsForCourse(int userId, UserRole role, int courseId) {
    // проверяем записан ли студент
    if(role == UserRole::Student) {
        if(!enrollmentRepository.isEnrolled(userId, courseId)) {
            return {};
        }
        return testRepository.getTestsForCourseForStudent(courseId, userId);
    }

    // преподавателю и админу можно в любом случае
    return testRepository.getTestsForCourse(courseId);
}

Test TestService::createTest(int courseId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) {
    if (title.empty()) {
        throw std::invalid_argument("Test title must not be empty");
    }
    if (maxAttempts < 0) {
        throw std::invalid_argument("Max attempts must not be negative");
    }
    if (timeLimitMinutes < 1 || timeLimitMinutes > 300) {
        throw std::invalid_argument("Time limit must be between 1 and 300 minutes");
    }

    return testRepository.createTest(courseId, title, normalizedStatus(status), deadlineAt, maxAttempts, timeLimitMinutes);
}

Test TestService::updateTest(int testId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) {
    if (title.empty()) {
        throw std::invalid_argument("Test title must not be empty");
    }
    if (maxAttempts < 0) {
        throw std::invalid_argument("Max attempts must not be negative");
    }
    if (timeLimitMinutes < 1 || timeLimitMinutes > 300) {
        throw std::invalid_argument("Time limit must be between 1 and 300 minutes");
    }

    return testRepository.updateTest(testId, title, normalizedStatus(status), deadlineAt, maxAttempts, timeLimitMinutes);
}

void TestService::deleteTest(int testId) {
    testRepository.deleteTest(testId);
}

std::optional<Test> TestService::getTestById(int testId) {
    return testRepository.getTestById(testId);
}
