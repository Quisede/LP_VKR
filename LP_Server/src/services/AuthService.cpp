//
//  AuthService.cpp
//  LP_Server
//  авторизация
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "AuthService.h"

AuthService::AuthService(UserRepository& userRepo,
                         PasswordHasher& hasher)
    : userRepository(userRepo),
      passwordHasher(hasher) {}

AuthResult AuthService::registerUser(const std::string &login, const std::string &password, UserRole role) {
    if(userRepository.exists(login)) {
        return {false, -1, role, "User already exists"};
    }
    
    std::string hash = passwordHasher.hash(password);
    User user = userRepository.createUser(login, hash, role);
    
    return {true, user.id, user.role, ""};
}

AuthResult AuthService::login(const std::string& login, const std::string& password) {
    auto userOpt = userRepository.findByLogin(login);
    
    if(!userOpt) {
        return {false, -1, UserRole::Student, "User not found"};
    }
    
    const User& user = *userOpt;
    
    if(!passwordHasher.verify(password, user.passwordHash)) {
        return {false, -1, user.role, "Inalid password"};
    }
    
    return {true, user.id, user.role, ""};
}
