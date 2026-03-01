//
//  CourseService.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#include "CourseService.h"

CourseService::CourseService(CourseRepository& repo, EnrollmentRepository& enrollRepo):
    courseRepository(repo),
    enrollmentRepository(enrollRepo) {}

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

bool CourseService::enrollStudent(int userId,
                                  UserRole role,
                                  int courseId) {

    if (role != UserRole::Student)
        return false;

    if (enrollmentRepository.isEnrolled(userId, courseId))
        return false;

    enrollmentRepository.enrollStudent(userId, courseId);
    return true;
}
