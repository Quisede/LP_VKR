#include "JwtService.h"
#include "../utils/Base64.h"
#include <sstream>

std::string JwtService::generateToken(int userId, const std::string& role) {
    std::string payload = std::to_string(userId) + ":" + role;
    return base64Encode(payload);
}

bool JwtService::validateToken(const std::string& token) {
    std::string decoded = base64Decode(token);
    size_t separator = decoded.find(":");

    if (separator == std::string::npos || separator == 0 || separator == decoded.size() - 1) {
        return false;
    }

    std::string userId = decoded.substr(0, separator);
    std::string role = decoded.substr(separator + 1);

    for (char c : userId) {
        if (c < '0' || c > '9') {
            return false;
        }
    }

    return role == "Student" || role == "Teacher" || role == "Admin";
}

int JwtService::extractUserId(const std::string& token) {
    std::string decoded = base64Decode(token);
    return std::stoi(decoded.substr(0, decoded.find(":")));
}

std::string JwtService::extractRole(const std::string& token) {
    std::string decoded = base64Decode(token);
    return decoded.substr(decoded.find(":") + 1);
}
