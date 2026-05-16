#pragma once

#include <vector>

#include "../models/AdminOverview.h"
#include "../models/AdminAuditEvent.h"
#include "../repositories/AdminAuditRepository.h"
#include "../models/User.h"
#include "../repositories/CourseRepository.h"
#include "../repositories/UserRepository.h"

class AdminService {
public:
    AdminService(
        UserRepository& userRepository,
        CourseRepository& courseRepository,
        AdminAuditRepository& auditRepository);

    std::vector<User> getAllUsers(int currentUserId, UserRole role);
    AdminOverview getOverview(int currentUserId, UserRole role);
    std::vector<AdminAuditEvent> getAuditEvents(int currentUserId, UserRole role, int limit);
    void recordAuditEvent(
        int currentUserId,
        UserRole currentRole,
        const std::string& action,
        const std::string& targetType,
        int targetId,
        const std::string& details);
    User updateUserRole(int currentUserId, UserRole currentRole, int targetUserId, UserRole newRole);
    void deleteUser(int currentUserId, UserRole currentRole, int targetUserId);

private:
    UserRepository& userRepository;
    CourseRepository& courseRepository;
    AdminAuditRepository& auditRepository;
};
