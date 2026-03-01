//
//  CourseService.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include <vector>
#include "../models/Course.h"
#include "../models/User.h"
#include "../repositories/CourseRepository.h"
#include "../repositories/EnrollmentRepository.h"

class CourseService {
public:
    CourseService(CourseRepository& repo, EnrollmentRepository& enrollRepo);
    
    std::vector<Course> getCoursesForUser(int userId, UserRole role);
    bool enrollStudent(int userId, UserRole role, int courseId);
    
private:
    CourseRepository& courseRepository;
    EnrollmentRepository& enrollmentRepository;
};
