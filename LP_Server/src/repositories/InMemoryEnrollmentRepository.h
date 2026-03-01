//
//  InMemoryEnrollmentRepository.h
//  LP_Server
//
//  Created by Даниил Дружинин on 15.02.2026.
//

#pragma once

#include "EnrollmentRepository.h"
#include <vector>
#include <utility>

class InMemoryEnrollmentRepository : public EnrollmentRepository {
public:
    void enrollStudent(int studentId, int courseId) override;
    virtual std::vector<int> getCoursesForStudent(int studentId) override;
    virtual bool isEnrolled(int studentId, int courseId) override;
    
private:
    std::vector<std::pair<int, int>> enrollments;
};
