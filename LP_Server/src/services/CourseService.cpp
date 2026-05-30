//
//  CourseService.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#include "CourseService.h"
#include <algorithm>
#include <stdexcept>

namespace {

std::string trimCopy(const std::string& value) {
    const auto begin = value.find_first_not_of(" \t\n\r");
    if (begin == std::string::npos) {
        return "";
    }

    const auto end = value.find_last_not_of(" \t\n\r");
    return value.substr(begin, end - begin + 1);
}

std::string roleToString(UserRole role) {
    switch (role) {
        case UserRole::Student: return "Student";
        case UserRole::Teacher: return "Teacher";
        case UserRole::Admin: return "Admin";
    }

    return "Student";
}

}

CourseService::CourseService(
    CourseRepository& repo,
    EnrollmentRepository& enrollRepo,
    UserRepository& userRepo):
    courseRepository(repo),
    enrollmentRepository(enrollRepo),
    userRepository(userRepo) {}

std::vector<Course> CourseService::getAllCourses() {
    return courseRepository.getAllCourses();
}

std::optional<Course> CourseService::getCourseById(int courseId) {
    return courseRepository.getCourseById(courseId);
}

bool CourseService::isStudentEnrolled(int userId, int courseId) {
    return enrollmentRepository.isEnrolled(userId, courseId);
}

bool CourseService::canManageCourse(int userId, UserRole role, int courseId) {
    auto course = getCourseById(courseId);
    if (!course.has_value()) {
        return false;
    }

    if (role == UserRole::Admin) {
        return true;
    }

    return role == UserRole::Teacher && course->teacherId == userId;
}

Course CourseService::createCourse(
    int teacherId,
    const std::string& title,
    const std::string& description) {
    std::string normalizedTitle = trimCopy(title);
    std::string normalizedDescription = trimCopy(description);

    if (normalizedTitle.empty()) {
        throw std::invalid_argument("Course title must not be empty");
    }

    if (normalizedDescription.empty()) {
        throw std::invalid_argument("Course description must not be empty");
    }

    return courseRepository.createCourse(normalizedTitle, normalizedDescription, teacherId);
}

Course CourseService::updateCourse(
    int courseId,
    const std::string& title,
    const std::string& description) {
    std::string normalizedTitle = trimCopy(title);
    std::string normalizedDescription = trimCopy(description);

    if (normalizedTitle.empty()) {
        throw std::invalid_argument("Course title must not be empty");
    }

    if (normalizedDescription.empty()) {
        throw std::invalid_argument("Course description must not be empty");
    }

    if (!courseRepository.getCourseById(courseId).has_value()) {
        throw std::invalid_argument("Course not found");
    }

    return courseRepository.updateCourse(courseId, normalizedTitle, normalizedDescription);
}

void CourseService::deleteCourse(int courseId) {
    if (!courseRepository.getCourseById(courseId).has_value()) {
        throw std::invalid_argument("Course not found");
    }

    courseRepository.deleteCourse(courseId);
}

std::vector<CourseStudent> CourseService::getStudentsForCourse(
    int userId,
    UserRole role,
    int courseId) {
    if (role != UserRole::Teacher && role != UserRole::Admin) {
        throw std::invalid_argument("Only teachers can view course students");
    }

    if (!getCourseById(courseId).has_value()) {
        throw std::invalid_argument("Course not found");
    }

    if (!canManageCourse(userId, role, courseId)) {
        throw std::invalid_argument("You can view students only for your own courses");
    }

    return userRepository.getStudentsForCourse(courseId);
}

std::vector<Course> CourseService::getCoursesForUser(int userId, UserRole role) {
//    switch (role) {
//        case UserRole::Student:
//            return courseRepository.getCoursesForStudent(userId);
//
//        case UserRole::Teacher:
//            return courseRepository.getCoursesForTeacher(userId);
//
//        case UserRole::Admin:
//            return courseRepository.getAllCourses();
//        }
//
//        return {};
    if (role == UserRole::Student) {
        auto courseIds = enrollmentRepository.getCoursesForStudent(userId);
        return courseRepository.getCoursesByIds(courseIds); // добавим метод
    }

    if (role == UserRole::Teacher) {
        return courseRepository.getCoursesForTeacher(userId);
    }

    return courseRepository.getAllCourses();
}

EnrollmentResult CourseService::enrollStudent(int userId,
                                              UserRole role,
                                              int courseId) {

    if (role != UserRole::Student) {
        return {
            EnrollmentStatus::ForbiddenRole,
            "Only students can enroll in courses"
        };
    }

    if (!courseRepository.getCourseById(courseId).has_value()) {
        return {
            EnrollmentStatus::CourseNotFound,
            "Course not found"
        };
    }

    if (enrollmentRepository.isEnrolled(userId, courseId)) {
        return {
            EnrollmentStatus::AlreadyEnrolled,
            "Student is already enrolled in this course"
        };
    }

    enrollmentRepository.enrollStudent(userId, courseId);
    return {
        EnrollmentStatus::Success,
        "Student enrolled successfully"
    };
}

std::vector<Course> CourseService::getCoursesPaged(int userId, UserRole role, int page, int limit) {

    int offset = (page - 1) * limit;

    return courseRepository.getCoursesPaged(userId, roleToString(role), limit, offset);
}
