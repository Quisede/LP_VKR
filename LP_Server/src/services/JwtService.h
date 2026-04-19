#pragma once

#include <string>

class JwtService {
    public:
        std::string generateToken(int userId, const std::string& role);
        bool validateToken(const std::string& token);
        int extractUserId(const std::string& token);
        std::string extractRole(const std::string& token);
};