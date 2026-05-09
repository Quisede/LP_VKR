//
//  UserRepository.h
//  LP_Server
//  Обработчик пользователей
//  Created by Даниил Дружинин on 08.01.2026.
//
#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../models/User.h"
#include "../models/CourseStudent.h"

class UserRepository {
public:
    virtual ~UserRepository() = default; // деструктор
    
    virtual std::optional<User> findByLogin(const std::string& login) = 0; // поиск пользователя по логину
    virtual bool exists(const std::string& login) = 0; /* проверка наличия пользователя с таким логином */
    virtual User createUser(const std::string& login, const std::string& passwordHash, UserRole role) = 0; /* сохранение нового пользователя */
    virtual std::vector<User> getAllUsers() = 0;
    virtual User updateUserRole(int userId, UserRole role) = 0;
    virtual void deleteUser(int userId) = 0;
    virtual std::vector<CourseStudent> getStudentsForCourse(int courseId) = 0;
};
