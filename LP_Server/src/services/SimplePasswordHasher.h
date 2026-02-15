//
//  SimplePasswordHasher.h
//  LP_Server
//
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "PasswordHasher.h"
#include <string>
#include <functional>

class SimplePasswordHasher : public PasswordHasher {
public:
    std::string hash(const std::string& password) override {
        return std::to_string(std::hash<std::string>{}(password));
    }

    bool verify(const std::string& password,
                const std::string& hash) override {
        return this->hash(password) == hash;
    }
};
