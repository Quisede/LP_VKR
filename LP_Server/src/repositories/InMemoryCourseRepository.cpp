//
//  InMemoryCourseRepository.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#include "InMemoryCourseRepository.h"

#include <algorithm>
#include <stdexcept>

InMemoryCourseRepository::InMemoryCourseRepository() {
    courses.push_back({1, "C++ Basics", "Intro to C++", 10, "Иван Петров", 6, 2, 12});
    courses.push_back({2, "OOP Design", "Design principles", 10, "Иван Петров", 4, 1, 8});
    courses.push_back({3, "Databases", "SQL and PostgreSQL", 20, "Мария Соколова", 7, 3, 15});
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

std::optional<Course> InMemoryCourseRepository::getCourseById(int courseId) {
    for (const auto& course : courses) {
        if (course.id == courseId) {
            return course;
        }
    }

    return std::nullopt;
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

Course InMemoryCourseRepository::createCourse(
    const std::string& title,
    const std::string& description,
    int teacherId) {
    int nextId = courses.empty() ? 1 : courses.back().id + 1;
    Course course{nextId, title, description, teacherId, "Преподаватель"};
    courses.push_back(course);
    return course;
}

Course InMemoryCourseRepository::updateCourse(
    int courseId,
    const std::string& title,
    const std::string& description) {
    for (auto& course : courses) {
        if (course.id == courseId) {
            course.title = title;
            course.description = description;
            return course;
        }
    }

    throw std::runtime_error("Course not found");
}

void InMemoryCourseRepository::deleteCourse(int courseId) {
    auto it = std::remove_if(courses.begin(), courses.end(), [courseId](const Course& course) {
        return course.id == courseId;
    });

    if (it == courses.end()) {
        throw std::runtime_error("Course not found");
    }

    courses.erase(it, courses.end());
}
