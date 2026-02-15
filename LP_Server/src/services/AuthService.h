//
//  AuthService.h
//  LP_Server
//  авторизация
//  Created by Даниил Дружинин on 08.01.2026.
//
#pragma once

#include <string>
#include "../models/AuthResult.h"
#include "../repositories/UserRepository.h"
#include "PasswordHasher.h"

class AuthService {
public:
    AuthService(UserRepository& userRepo, PasswordHasher& hasher); /* конструктор */
    
    AuthResult login(const std::string& login, const std::string& password); /* проверка логина и пароля (вход) */
    
    AuthResult registerUser(const std::string& login, const std::string& password, UserRole role); /* регистрация нового пользователя */
    
private:
    UserRepository& userRepository;
    PasswordHasher& passwordHasher;
};
