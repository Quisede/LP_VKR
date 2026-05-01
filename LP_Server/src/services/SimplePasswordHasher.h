//
//  SimplePasswordHasher.h
//  LP_Server
//
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "PasswordHasher.h"
#include <string>
#include <cstdint>

class SimplePasswordHasher : public PasswordHasher {
public:
    std::string hash(const std::string& password) override {
        // Стабильный FNV-1a, чтобы одинаково работать на macOS и Linux/Docker.
        std::uint64_t value = 14695981039346656037ULL;
        for (unsigned char ch : password) {
            value ^= static_cast<std::uint64_t>(ch);
            value *= 1099511628211ULL;
        }
        return std::to_string(value);
    }

    bool verify(const std::string& password,
                const std::string& hash) override {
        return this->hash(password) == hash;
    }
};
