//
//  EnrollmentRepository.h
//  LP_Server
//
//  хранит свзяи
//
//  Created by Даниил Дружинин on 15.02.2026.
//

#pragma once

#include <vector>

class EnrollmentRepository {
public:
    virtual ~EnrollmentRepository() = default;
    
    virtual void enrollStudent(int studentId, int courseId) = 0; // регистрация пользователя на курс
    virtual std::vector<int> getCoursesForStudent(int studentId) = 0; // получить курсы для студента
    virtual bool isEnrolled(int studentId, int courseId) = 0; // уже зарегистрирован?
};
