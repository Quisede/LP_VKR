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
    std::vector<Course> getCoursesByIds(const std::vector<int>& ids) override;
    std::optional<Course> getCourseById(int courseId) override;
    std::vector<Course> getCoursesPaged(int userId, const std::string& role, int limit, int offset) override;
    std::vector<Course> getAllCourses() override;
    Course createCourse(const std::string& title, const std::string& description, int teacherId) override;
    Course updateCourse(int courseId, const std::string& title, const std::string& description) override;
    void deleteCourse(int courseId) override;
private:
    std::vector<Course> courses;
};
