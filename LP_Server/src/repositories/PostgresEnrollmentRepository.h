#pragma once

#include "EnrollmentRepository.h"
#include "../database/PostgresConnection.h"

class PostgresEnrollmentRepository : public EnrollmentRepository {
public:
    PostgresEnrollmentRepository(PostgresConnection& conn);

    void enrollStudent(int studentId, int courseId) override;
    std::vector<int> getCoursesForStudent(int studentId) override;
    bool isEnrolled(int studentId, int courseId) override;

private:
    PostgresConnection& connection;
};
