//
//  UserRepository.h
//  LP_Server
//  Обработчик пользователей
//  Created by Даниил Дружинин on 08.01.2026.
//
#pragma once

#include <optional>
#include <string>
#include "../models/User.h"

class UserRepository {
public:
    virtual ~UserRepository() = default; // деструктор
    
    virtual std::optional<User> findByLogin(const std::string& login) = 0; // поиск пользователя по логину
    virtual bool exists(const std::string& login) = 0; /* проверка наличия пользователя с таким логином */
    virtual User createUser(const std::string& login, const std::string& passwordHash, UserRole role) = 0; /* сохранение нового пользователя */
};
