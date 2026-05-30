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

    std::optional<User> findById(int userId) override {
        for (const auto& user : users) {
            if (user.id == userId) {
                return user;
            }
        }
        return std::nullopt;
    }
    
    bool exists(const std::string& login) override {
        return findByLogin(login).has_value();
    }
    
    User createUser(
        const std::string& login,
        const std::string& passwordHash,
        UserRole role,
        const std::string& firstName = "",
        const std::string& lastName = "",
        const std::string& groupName = "",
        const std::string& email = "",
        const std::string& phone = "") override {
        User user {
            ++currentId,
            login,
            passwordHash,
            role,
            firstName.empty() ? login : firstName,
            lastName,
            groupName,
            email,
            phone
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

    void updatePasswordHash(int userId, const std::string& passwordHash) override {
        for (auto &user : users) {
            if (user.id == userId) {
                user.passwordHash = passwordHash;
                return;
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

    std::vector<std::string> getGroups() override {
        return groups;
    }

    std::string createGroup(const std::string& groupName) override {
        if (std::find(groups.begin(), groups.end(), groupName) == groups.end()) {
            groups.push_back(groupName);
        }
        return groupName;
    }

    std::string renameGroup(const std::string& oldName, const std::string& newName) override {
        for (auto& group : groups) {
            if (group == oldName) {
                group = newName;
            }
        }
        for (auto& user : users) {
            if (user.groupName == oldName) {
                user.groupName = newName;
            }
        }
        if (std::find(groups.begin(), groups.end(), newName) == groups.end()) {
            groups.push_back(newName);
        }
        return newName;
    }

    void deleteGroup(const std::string& groupName) override {
        groups.erase(
            std::remove(groups.begin(), groups.end(), groupName),
            groups.end());
        for (auto& user : users) {
            if (user.groupName == groupName) {
                user.groupName.clear();
            }
        }
    }
    
private:
    std::vector<User> users;
    std::vector<std::string> groups;
    int currentId = 0;
};
