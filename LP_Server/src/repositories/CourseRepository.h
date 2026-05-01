//
//  CourseRepository.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include <optional>
#include <vector>
#include <string>
#include "../models/Course.h"

class CourseRepository {
public:
    virtual std::vector<Course> getCoursesForStudent(int userId) = 0;

    virtual std::vector<Course> getCoursesForTeacher(int teacherId) = 0;

    virtual std::vector<Course> getCoursesByIds(const std::vector<int>& courseIds) = 0;

    virtual std::optional<Course> getCourseById(int courseId) = 0;

    virtual std::vector<Course> getCoursesPaged(int userId, const std::string& role, int limit, int offset) = 0;
    
    virtual std::vector<Course> getAllCourses() = 0;

    virtual Course createCourse(
        const std::string& title,
        const std::string& description,
        int teacherId) = 0;

    virtual ~CourseRepository() = default;
};
