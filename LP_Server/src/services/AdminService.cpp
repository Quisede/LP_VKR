#include "AdminService.h"

#include <stdexcept>

namespace {
std::string roleToString(UserRole role)
{
    switch (role) {
        case UserRole::Student: return "Student";
        case UserRole::Teacher: return "Teacher";
        case UserRole::Admin: return "Admin";
    }
    return "Unknown";
}
}

AdminService::AdminService(
    UserRepository& userRepository,
    CourseRepository& courseRepository,
    AdminAuditRepository& auditRepository)
    : userRepository(userRepository),
      courseRepository(courseRepository),
      auditRepository(auditRepository) {}

std::vector<User> AdminService::getAllUsers(int, UserRole role)
{
    if (role != UserRole::Admin) {
        throw std::invalid_argument("Only admins can view users");
    }

    return userRepository.getAllUsers();
}

AdminOverview AdminService::getOverview(int, UserRole role)
{
    if (role != UserRole::Admin) {
        throw std::invalid_argument("Only admins can view system overview");
    }

    const auto users = userRepository.getAllUsers();
    const auto courses = courseRepository.getAllCourses();

    AdminOverview overview;
    overview.totalUsers = static_cast<int>(users.size());
    overview.coursesCount = static_cast<int>(courses.size());

    for (const auto &user : users) {
        switch (user.role) {
            case UserRole::Student:
                ++overview.studentsCount;
                break;
            case UserRole::Teacher:
                ++overview.teachersCount;
                break;
            case UserRole::Admin:
                ++overview.adminsCount;
                break;
        }
    }

    for (const auto &course : courses) {
        overview.lessonsCount += course.lessonsCount;
        overview.testsCount += course.testsCount;
        overview.enrollmentsCount += course.studentsCount;
    }

    return overview;
}

std::vector<AdminAuditEvent> AdminService::getAuditEvents(int, UserRole role, int limit)
{
    if (role != UserRole::Admin) {
        throw std::invalid_argument("Only admins can view audit log");
    }

    return auditRepository.getRecentEvents(limit);
}

void AdminService::recordAuditEvent(
    int currentUserId,
    UserRole currentRole,
    const std::string& action,
    const std::string& targetType,
    int targetId,
    const std::string& details)
{
    if (currentRole != UserRole::Admin) {
        throw std::invalid_argument("Only admins can record audit events");
    }

    auditRepository.recordEvent(currentUserId, action, targetType, targetId, details);
}

User AdminService::updateUserRole(int currentUserId, UserRole currentRole, int targetUserId, UserRole newRole)
{
    if (currentRole != UserRole::Admin) {
        throw std::invalid_argument("Only admins can update users");
    }

    if (currentUserId == targetUserId) {
        throw std::invalid_argument("Admin cannot change own role");
    }

    User user = userRepository.updateUserRole(targetUserId, newRole);
    auditRepository.recordEvent(
        currentUserId,
        "user.role_updated",
        "user",
        targetUserId,
        "Role for " + user.login + " changed to " + roleToString(newRole));
    return user;
}

void AdminService::deleteUser(int currentUserId, UserRole currentRole, int targetUserId)
{
    if (currentRole != UserRole::Admin) {
        throw std::invalid_argument("Only admins can delete users");
    }

    if (currentUserId == targetUserId) {
        throw std::invalid_argument("Admin cannot delete own account");
    }

    std::string targetLogin = "user #" + std::to_string(targetUserId);
    for (const auto& user : userRepository.getAllUsers()) {
        if (user.id == targetUserId) {
            targetLogin = user.login;
            break;
        }
    }

    userRepository.deleteUser(targetUserId);
    auditRepository.recordEvent(
        currentUserId,
        "user.deleted",
        "user",
        targetUserId,
        "Deleted user " + targetLogin);
}
