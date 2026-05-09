#include "AttemptService.h"
#include <stdexcept>

AttemptService::AttemptService(AttemptRepository& repo, CourseService& courseService)
    : attemptRepository(repo),
      courseService(courseService) {}

std::vector<Attempt> AttemptService::getAttemptsForUser(
    int currentUserId,
    UserRole role,
    int requestedUserId) {

    // проверка доступа
    if (currentUserId != requestedUserId &&
        role != UserRole::Admin) {

        return {}; // можно позже сделать exception
    }

    return attemptRepository.getAttemptsForUser(requestedUserId);
}

CourseAnalytics AttemptService::getCourseAnalytics(int currentUserId, UserRole role, int courseId) {
    if (role != UserRole::Teacher && role != UserRole::Admin) {
        throw std::invalid_argument("Only teachers can view course analytics");
    }

    if (!courseService.getCourseById(courseId).has_value()) {
        throw std::invalid_argument("Course not found");
    }

    if (!courseService.canManageCourse(currentUserId, role, courseId)) {
        throw std::invalid_argument("You can view analytics only for your own courses");
    }

    return attemptRepository.getCourseAnalytics(courseId);
}

std::vector<StudentCourseAttempt> AttemptService::getStudentCourseAttempts(
    int currentUserId,
    UserRole role,
    int courseId,
    int studentId) {
    if (role != UserRole::Teacher && role != UserRole::Admin) {
        throw std::invalid_argument("Only teachers can view student attempts");
    }

    if (!courseService.getCourseById(courseId).has_value()) {
        throw std::invalid_argument("Course not found");
    }

    if (!courseService.canManageCourse(currentUserId, role, courseId)) {
        throw std::invalid_argument("You can view attempts only for your own courses");
    }

    return attemptRepository.getStudentCourseAttempts(courseId, studentId);
}
