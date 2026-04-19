//
//  InMemoryCourseRepository.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#include "InMemoryCourseRepository.h"

InMemoryCourseRepository::InMemoryCourseRepository() {
    courses.push_back({1, "C++ Basics", "Intro to C++", 10});
    courses.push_back({2, "OOP Design", "Design principles", 10});
    courses.push_back({3, "Databases", "SQL and PostgreSQL", 20});
}

std::vector<Course> InMemoryCourseRepository::getCoursesForStudent(int studentId) {
    return courses;
}

std::vector<Course> InMemoryCourseRepository::getCoursesForTeacher(int teacherId) {
    std::vector<Course> result;
    for (const auto& c : courses) {
        if (c.teacherId == teacherId) {
            result.push_back(c);
        }
    }
    return result;
}

std::vector<Course> InMemoryCourseRepository::getAllCourses() {
    return courses;
}

std::vector<Course> InMemoryCourseRepository::getCoursesByIds(
    const std::vector<int>& ids) {
    std::vector<Course> result;

    for (const auto& course : courses) {
        for (int id : ids) {
            if (course.id == id) {
                result.push_back(course);
                break;
            }
        }
    }

    return result;
}

std::vector<Course> InMemoryCourseRepository::getCoursesPaged(
    int userId,
    const std::string& role,
    int limit,
    int offset) {
    std::vector<Course> availableCourses;

    if (role == "Teacher") {
        availableCourses = getCoursesForTeacher(userId);
    } else {
        availableCourses = getAllCourses();
    }

    std::vector<Course> page;
    int total = static_cast<int>(availableCourses.size());

    for (int i = offset; i < offset + limit && i < total; ++i) {
        page.push_back(availableCourses[i]);
    }

    return page;
}
