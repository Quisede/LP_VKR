//
//  CourseService.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include <vector>
#include <string>
#include "../models/Course.h"
#include "../models/User.h"
#include "../repositories/CourseRepository.h"
#include "../repositories/EnrollmentRepository.h"

enum class EnrollmentStatus {
    Success,
    ForbiddenRole,
    CourseNotFound,
    AlreadyEnrolled
};

struct EnrollmentResult {
    EnrollmentStatus status;
    std::string message;

    bool success() const {
        return status == EnrollmentStatus::Success;
    }
};

class CourseService {
public:
    CourseService(CourseRepository& repo, EnrollmentRepository& enrollRepo);
    
    std::vector<Course> getAllCourses();
    std::vector<Course> getCoursesForUser(int userId, UserRole role);
    EnrollmentResult enrollStudent(int userId, UserRole role, int courseId);
    std::vector<Course> getCoursesPaged(int userId, UserRole role, int page, int limit);
    
private:
    CourseRepository& courseRepository;
    EnrollmentRepository& enrollmentRepository;
};
