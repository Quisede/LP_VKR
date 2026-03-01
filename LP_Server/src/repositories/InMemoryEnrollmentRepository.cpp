//
//  InMemoryEnrollmentRepository.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 15.02.2026.
//

#include "InMemoryEnrollmentRepository.h"

void InMemoryEnrollmentRepository::enrollStudent(int studentId, int courseId) {
    enrollments.emplace_back(studentId, courseId);
}

std::vector<int> InMemoryEnrollmentRepository::getCoursesForStudent(int studentId) {
    std::vector<int> result;
    
    for(const auto& e : enrollments) {
        if(e.first == studentId) {
            result.push_back(e.second);
        }
    }
    
    return result;
}

bool InMemoryEnrollmentRepository::isEnrolled(int studentId, int courseId) {
    for (const auto & e : enrollments) {
        if (e.first == studentId && e.second == courseId) {
            return true;
        }
    }
    
    return false;
}
