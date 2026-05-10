#pragma once

#include <vector>

#include "../models/AdminOverview.h"
#include "../models/User.h"
#include "../repositories/CourseRepository.h"
#include "../repositories/UserRepository.h"

class AdminService {
public:
    AdminService(UserRepository& userRepository, CourseRepository& courseRepository);

    std::vector<User> getAllUsers(int currentUserId, UserRole role);
    AdminOverview getOverview(int currentUserId, UserRole role);
    User updateUserRole(int currentUserId, UserRole currentRole, int targetUserId, UserRole newRole);
    void deleteUser(int currentUserId, UserRole currentRole, int targetUserId);

private:
    UserRepository& userRepository;
    CourseRepository& courseRepository;
};
