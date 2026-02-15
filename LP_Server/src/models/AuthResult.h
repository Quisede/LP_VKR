//
//  AuthResult.h
//  LP_Server
//  Модель результата
//
//
//  Created by Даниил Дружинин on 08.01.2026.
//
#pragma once

#include <string>

enum class UserRole; // предварительное объявление

struct AuthResult {
    bool success; // успешность
    int userId; // идентификатор пользователя
    UserRole role; // роль пользователя;
    std::string errorMessage; // сообщение об ошибке
};
