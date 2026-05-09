#include "AdminService.h"

#include <stdexcept>

AdminService::AdminService(UserRepository& userRepository)
    : userRepository(userRepository) {}

std::vector<User> AdminService::getAllUsers(int, UserRole role)
{
    if (role != UserRole::Admin) {
        throw std::invalid_argument("Only admins can view users");
    }

    return userRepository.getAllUsers();
}

User AdminService::updateUserRole(int currentUserId, UserRole currentRole, int targetUserId, UserRole newRole)
{
    if (currentRole != UserRole::Admin) {
        throw std::invalid_argument("Only admins can update users");
    }

    if (currentUserId == targetUserId) {
        throw std::invalid_argument("Admin cannot change own role");
    }

    return userRepository.updateUserRole(targetUserId, newRole);
}

void AdminService::deleteUser(int currentUserId, UserRole currentRole, int targetUserId)
{
    if (currentRole != UserRole::Admin) {
        throw std::invalid_argument("Only admins can delete users");
    }

    if (currentUserId == targetUserId) {
        throw std::invalid_argument("Admin cannot delete own account");
    }

    userRepository.deleteUser(targetUserId);
}
