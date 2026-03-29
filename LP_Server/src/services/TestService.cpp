#include "TestService.h"

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