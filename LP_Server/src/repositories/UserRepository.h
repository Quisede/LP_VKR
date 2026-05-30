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
    virtual std::optional<User> findById(int userId) = 0;
    virtual bool exists(const std::string& login) = 0; /* проверка наличия пользователя с таким логином */
    virtual User createUser(
        const std::string& login,
        const std::string& passwordHash,
        UserRole role,
        const std::string& firstName = "",
        const std::string& lastName = "",
        const std::string& groupName = "",
        const std::string& email = "",
        const std::string& phone = "") = 0; /* сохранение нового пользователя */
    virtual std::vector<User> getAllUsers() = 0;
    virtual User updateUserRole(int userId, UserRole role) = 0;
    virtual void updatePasswordHash(int userId, const std::string& passwordHash) = 0;
    virtual void deleteUser(int userId) = 0;
    virtual std::vector<CourseStudent> getStudentsForCourse(int courseId) = 0;
    virtual std::vector<std::string> getGroups() = 0;
    virtual std::string createGroup(const std::string& groupName) = 0;
    virtual std::string renameGroup(const std::string& oldName, const std::string& newName) = 0;
    virtual void deleteGroup(const std::string& groupName) = 0;
};
