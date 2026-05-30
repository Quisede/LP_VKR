//
//  CourseService.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include <optional>
#include <vector>
#include <string>
#include "../models/Course.h"
#include "../models/CourseStudent.h"
#include "../models/User.h"
#include "../repositories/CourseRepository.h"
#include "../repositories/EnrollmentRepository.h"
#include "../repositories/UserRepository.h"

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
    CourseService(CourseRepository& repo, EnrollmentRepository& enrollRepo, UserRepository& userRepo);
    
    std::vector<Course> getAllCourses();
    std::vector<Course> getCoursesForUser(int userId, UserRole role);
    std::optional<Course> getCourseById(int courseId);
    bool isStudentEnrolled(int userId, int courseId);
    bool canManageCourse(int userId, UserRole role, int courseId);
    Course createCourse(int teacherId, const std::string& title, const std::string& description);
    Course updateCourse(int courseId, const std::string& title, const std::string& description);
    void deleteCourse(int courseId);
    std::vector<CourseStudent> getStudentsForCourse(int userId, UserRole role, int courseId);
    EnrollmentResult enrollStudent(int userId, UserRole role, int courseId);
    std::vector<Course> getCoursesPaged(int userId, UserRole role, int page, int limit);
    
private:
    CourseRepository& courseRepository;
    EnrollmentRepository& enrollmentRepository;
    UserRepository& userRepository;
};
