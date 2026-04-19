#pragma once

#include "CourseRepository.h"
#include "../database/PostgresConnection.h"

class PostgresCourseRepository : public CourseRepository {
public:
    PostgresCourseRepository(PostgresConnection& conn);

    std::vector<Course> getCoursesForStudent(int userId) override;
    std::vector<Course> getCoursesForTeacher(int teacherId) override;
    std::vector<Course> getCoursesByIds(const std::vector<int>& courseIds) override;
    std::vector<Course> getAllCourses() override;
    std::vector<Course> getCoursesPaged(int userId, const std::string& role, int limit, int offset) override;

private:
    PostgresConnection& connection;
};
