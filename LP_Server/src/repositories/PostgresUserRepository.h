#pragma once

#include "UserRepository.h"
#include "../database/PostgresConnection.h"

class PostgresUserRepository : public UserRepository {
    public:
        PostgresUserRepository(PostgresConnection& conn);

        std::optional<User> findByLogin(const std::string& login) override;
        std::optional<User> findById(int userId) override;
        bool exists(const std::string& login) override;

        User createUser(const std::string& login,
                    const std::string& passwordHash,
                    UserRole role) override;
        std::vector<User> getAllUsers() override;
        User updateUserRole(int userId, UserRole role) override;
        void updatePasswordHash(int userId, const std::string& passwordHash) override;
        void deleteUser(int userId) override;
        std::vector<CourseStudent> getStudentsForCourse(int courseId) override;
    private:
        PostgresConnection& connection;
};
