#pragma once

#include <vector>

#include "../models/User.h"
#include "../repositories/UserRepository.h"

class AdminService {
public:
    explicit AdminService(UserRepository& userRepository);

    std::vector<User> getAllUsers(int currentUserId, UserRole role);
    User updateUserRole(int currentUserId, UserRole currentRole, int targetUserId, UserRole newRole);
    void deleteUser(int currentUserId, UserRole currentRole, int targetUserId);

private:
    UserRepository& userRepository;
};
