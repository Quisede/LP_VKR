//
//  InMemoryCourseRepository.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include "CourseRepository.h"
#include <vector>

class InMemoryCourseRepository : public CourseRepository {
public:
    InMemoryCourseRepository();
    
    std::vector<Course> getCoursesForStudent(int studentId) override;
    std::vector<Course> getCoursesForTeacher(int teacherId) override;
    std::vector<Course> getAllCourses() override;

private:
    std::vector<Course> courses;
};
