#pragma once

#include <string>

struct AdminAuditEvent {
    int id = -1;
    int adminId = -1;
    std::string adminLogin;
    std::string action;
    std::string targetType;
    int targetId = -1;
    std::string details;
    std::string createdAt;
};
