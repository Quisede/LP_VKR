#include "TestService.h"
#include <stdexcept>

TestService::TestService(TestRepository& testRepo,
            EnrollmentRepository& enrollRepo)
    : testRepository(testRepo), enrollmentRepository(enrollRepo) {}

std::vector<Test> TestService::getTestsForCourse(int userId, UserRole role, int courseId) {
    // проверяем записан ли студент
    if(role == UserRole::Student) {
        if(!enrollmentRepository.isEnrolled(userId, courseId)) {
            return {};
        }
    }

    // преподавателю и админу можно в любом случае
    return testRepository.getTestsForCourse(courseId);
}

Test TestService::createTest(int courseId, const std::string& title) {
    if (title.empty()) {
        throw std::invalid_argument("Test title must not be empty");
    }

    return testRepository.createTest(courseId, title);
}

Test TestService::updateTest(int testId, const std::string& title) {
    if (title.empty()) {
        throw std::invalid_argument("Test title must not be empty");
    }

    return testRepository.updateTest(testId, title);
}

void TestService::deleteTest(int testId) {
    testRepository.deleteTest(testId);
}

std::optional<Test> TestService::getTestById(int testId) {
    return testRepository.getTestById(testId);
}
