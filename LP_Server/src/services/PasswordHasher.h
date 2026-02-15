//
//  PasswordHasher.h
//  LP_Server
//  интерфейс хэширования пароля
//  Created by Даниил Дружинин on 08.01.2026.
//
#pragma once

#include <string>

class PasswordHasher {
public:
    virtual ~PasswordHasher() = default;

    virtual std::string hash(const std::string& password) = 0;
    virtual bool verify(const std::string& password,
                        const std::string& hash) = 0;
};


