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
    std::vector<std::string> getGroups(int currentUserId, UserRole role);
    std::string createGroup(int currentUserId, UserRole role, const std::string& groupName);
    std::string renameGroup(int currentUserId, UserRole role, const std::string& oldName, const std::string& newName);
    void deleteGroup(int currentUserId, UserRole role, const std::string& groupName);
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
