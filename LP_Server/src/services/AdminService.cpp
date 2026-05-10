#include "AdminService.h"

#include <stdexcept>

AdminService::AdminService(UserRepository& userRepository, CourseRepository& courseRepository)
    : userRepository(userRepository),
      courseRepository(courseRepository) {}

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
