#pragma once

#include "CourseRepository.h"
#include "../database/PostgresConnection.h"

#include <optional>

class PostgresCourseRepository : public CourseRepository {
public:
    PostgresCourseRepository(PostgresConnection& conn);

    std::vector<Course> getCoursesForStudent(int userId) override;
    std::vector<Course> getCoursesForTeacher(int teacherId) override;
    std::vector<Course> getCoursesByIds(const std::vector<int>& courseIds) override;
    std::optional<Course> getCourseById(int courseId) override;
    std::vector<Course> getAllCourses() override;
    std::vector<Course> getCoursesPaged(int userId, const std::string& role, int limit, int offset) override;
    Course createCourse(const std::string& title, const std::string& description, int teacherId) override;

private:
    PostgresConnection& connection;
};
