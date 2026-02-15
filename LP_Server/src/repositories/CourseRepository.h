//
//  CourseRepository.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include <vector>
#include "../models/Course.h"

class CourseRepository {
public:
    virtual std::vector<Course> getCoursesForStudent(int studentId) = 0;
    virtual std::vector<Course> getCoursesForTeacher(int teacherId) = 0;
    virtual std::vector<Course> getAllCourses() = 0;
};
