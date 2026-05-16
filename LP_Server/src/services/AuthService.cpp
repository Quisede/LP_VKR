//
//  AuthService.cpp
//  LP_Server
//  авторизация
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "AuthService.h"

namespace {

AuthResult successResultFromUser(const User& user) {
    AuthResult result;
    result.success = true;
    result.userId = user.id;
    result.role = user.role;
    result.login = user.login;
    result.firstName = user.firstName;
    result.lastName = user.lastName;
    result.groupName = user.groupName;
    result.email = user.email;
    result.phone = user.phone;
    return result;
}

AuthResult failedResult(UserRole role, const std::string& message) {
    AuthResult result;
    result.success = false;
    result.userId = -1;
    result.role = role;
    result.errorMessage = message;
    return result;
}

}

AuthService::AuthService(UserRepository& userRepo,
                         PasswordHasher& hasher)
    : userRepository(userRepo),
      passwordHasher(hasher) {}

AuthResult AuthService::registerUser(const std::string &login, const std::string &password, UserRole role) {
    if(userRepository.exists(login)) {
        return failedResult(role, "User already exists");
    }
    
    std::string hash = passwordHasher.hash(password);
    User user = userRepository.createUser(login, hash, role);
    
    return successResultFromUser(user);
}

AuthResult AuthService::login(const std::string& login, const std::string& password) {
    auto userOpt = userRepository.findByLogin(login);
    
    if(!userOpt) {
        return failedResult(UserRole::Student, "User not found");
    }
    
    const User& user = *userOpt;
    
    if(!passwordHasher.verify(password, user.passwordHash)) {
        return failedResult(user.role, "Invalid password");
    }
    
    return successResultFromUser(user);
}

std::optional<User> AuthService::getProfile(int userId) {
    return userRepository.findById(userId);
}

bool AuthService::changePassword(
    int userId,
    const std::string& oldPassword,
    const std::string& newPassword) {
    auto userOpt = userRepository.findById(userId);
    if (!userOpt) {
        return false;
    }

    const User& user = *userOpt;
    if (!passwordHasher.verify(oldPassword, user.passwordHash)) {
        return false;
    }

    userRepository.updatePasswordHash(userId, passwordHasher.hash(newPassword));
    return true;
}
