//
//  CourseService.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#include "CourseService.h"

CourseService::CourseService(CourseRepository& repo):
    courseRepository(repo) {}

std::vector<Course> CourseService::getCoursesForUser(int userId, UserRole role) {
    switch (role) {
        case UserRole::Student:
            return courseRepository.getCoursesForStudent(userId);

        case UserRole::Teacher:
            return courseRepository.getCoursesForTeacher(userId);

        case UserRole::Admin:
            return courseRepository.getAllCourses();
        }

        return {};
}
