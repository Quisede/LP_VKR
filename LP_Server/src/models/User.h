//
//  User.h - пользователь
//  LP_Server
//  Хранит всех участников системы и их роль.
//  
//
//  Created by Даниил Дружинин on 08.01.2026.
//
#pragma once

#include <string>

// роль (студент, преподаватель, администратор)
enum class UserRole {
    Student,
    Teacher,
    Admin
};

struct User {
    int id; // идентификатор пользователя
    std::string login; // логин
    std::string passwordHash; // пароль (хэш)
    UserRole role; // роль
};
