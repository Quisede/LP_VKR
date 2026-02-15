//
//  InMemoryUserRepository.h
//  LP_Server
//  in-memory репозиторий пользователей
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "UserRepository.h"
#include <vector>

class InMemoryUserRepository : public UserRepository {
public:
    InMemoryUserRepository() = default;
    /* поиск по логину */
    std::optional<User> findByLogin(const std::string& login) {
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
    
private:
    std::vector<User> users;
    int currentId = 0;
};
