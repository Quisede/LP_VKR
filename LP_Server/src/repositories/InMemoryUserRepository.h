//
//  InMemoryUserRepository.h
//  LP_Server
//  in-memory репозиторий пользователей
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "UserRepository.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

class InMemoryUserRepository : public UserRepository {
public:
    InMemoryUserRepository() = default;
    /* поиск по логину */
    std::optional<User> findByLogin(const std::string& login) override {
        for(const auto& user : users) {
            if(user.login == login) {
                return user;
            }
        }
        return std::nullopt;
    }
    
    bool exists(const std::string& login) override {
        return findByLogin(login).has_value();
    }
    
    User createUser(const std::string& login, const std::string& passwordHash, UserRole role) override {
        User user {
            ++currentId,
            login,
            passwordHash,
            role
        };
        
        users.push_back(user);
        return user;
    }

    std::vector<User> getAllUsers() override {
        return users;
    }

    User updateUserRole(int userId, UserRole role) override {
        for (auto &user : users) {
            if (user.id == userId) {
                user.role = role;
                return user;
            }
        }

        throw std::runtime_error("User not found");
    }

    void deleteUser(int userId) override {
        users.erase(
            std::remove_if(
                users.begin(),
                users.end(),
                [userId](const User &user) { return user.id == userId; }),
            users.end());
    }

    std::vector<CourseStudent> getStudentsForCourse(int) override {
        return {};
    }
    
private:
    std::vector<User> users;
    int currentId = 0;
};
