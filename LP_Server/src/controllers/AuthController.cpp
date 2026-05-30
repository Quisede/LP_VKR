//
//  AuthController.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "AuthController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

namespace {

json profileToJson(const User& user, const std::string& role)
{
    const std::string fullName = user.lastName.empty() && user.firstName.empty()
        ? user.login
        : user.lastName + (user.lastName.empty() || user.firstName.empty() ? "" : " ") + user.firstName;

    return {
        {"userId", user.id},
        {"login", user.login},
        {"role", role},
        {"firstName", user.firstName},
        {"lastName", user.lastName},
        {"fullName", fullName},
        {"groupName", user.groupName},
        {"email", user.email},
        {"phone", user.phone}
    };
}

void fillAuthProfile(json& response, const AuthResult& result, const std::string& role)
{
    const std::string fullName = result.lastName.empty() && result.firstName.empty()
        ? result.login
        : result.lastName + (result.lastName.empty() || result.firstName.empty() ? "" : " ") + result.firstName;

    response["login"] = result.login;
    response["firstName"] = result.firstName;
    response["lastName"] = result.lastName;
    response["fullName"] = fullName;
    response["groupName"] = result.groupName;
    response["email"] = result.email;
    response["phone"] = result.phone;
    response["role"] = role;
}

std::string optionalProfileString(const json& body, const std::string& key)
{
    if (!body.contains(key) || body.at(key).is_null()) {
        return "";
    }

    if (!body.at(key).is_string()) {
        throw std::invalid_argument(key + " must be a string");
    }

    return body.at(key).get<std::string>();
}

}

AuthController::AuthController(AuthService& authService, JwtService& jwtService):
    authService(authService),
    jwtService(jwtService) {}

void AuthController::registerRoutes(httplib::Server &server) {
    /*
     @param this - захват текущий оюъект в лямбда функцию
     @param req - входящий http запрос
     @param res - исходящий http ответ ( в него записываем результат)
     */
    server.Post("/api/auth/login", [this](const httplib::Request& req, httplib::Response& res) {
        
        try {
            /* парсим json-тело запроса */
            auto body = json::parse(req.body);
            
            std::string login = controller_utils::requiredJsonString(body, "login");
            std::string password = controller_utils::requiredJsonString(body, "password");
            
            /* возвращаем структуру с результатом аутентификации */
            AuthResult result = authService.login(login, password);
            
            /* создаем json-объект для ответа */
            json responce;
            
            /* формирование json-ответа в зависимости от успешности аутентификации */
            if(result.success) {
                std::string role = roleToString(result.role);
                std::string token = jwtService.generateToken(result.userId, role);

                responce["success"] = true;
                responce["userId"] = result.userId;
                responce["token"] = token;
                fillAuthProfile(responce, result, role);
            } else {
                responce["success"] = false;
                responce["error"] = result.errorMessage;
            }
            
            /* ответ клиенту */
            res.set_content(responce.dump(), "application/json");
        } catch(const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
    
    // регистрация
    server.Post("/api/auth/register", [this](const httplib::Request& req, httplib::Response& res) {
        
        try {
            /* парсим json-тело запроса */
            auto body = json::parse(req.body);
            
            std::string login = controller_utils::requiredJsonString(body, "login");
            std::string password = controller_utils::requiredJsonString(body, "password");
            std::string firstName = optionalProfileString(body, "firstName");
            std::string lastName = optionalProfileString(body, "lastName");
            std::string groupName = optionalProfileString(body, "groupName");
            std::string email = optionalProfileString(body, "email");
            std::string phone = optionalProfileString(body, "phone");
            
            /* возвращаем структуру с результатом аутентификации */
            AuthResult result = authService.registerUser(
                login,
                password,
                UserRole::Student,
                firstName,
                lastName,
                groupName,
                email,
                phone);
            
            /* создаем json-объект для ответа */
            json responce;
            
            /* формирование json-ответа в зависимости от успешности аутентификации */
            if(result.success) {
                std::string role = "Student";
                std::string token = jwtService.generateToken(result.userId, role);

                responce["success"] = true;
                responce["userId"] = result.userId;
                responce["token"] = token;
                fillAuthProfile(responce, result, role);
            } else {
                responce["success"] = false;
                responce["error"] = result.errorMessage;
            }
            
            /* ответ клиенту */
            res.set_content(responce.dump(), "application/json");
        } catch(const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Get("/api/auth/me", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto userOpt = authService.getProfile(auth.userId);
            if (!userOpt) {
                throw controller_utils::HttpError(404, "User not found");
            }

            res.set_content(
                profileToJson(*userOpt, roleToString(userOpt->role)).dump(),
                "application/json");
        } catch(const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post("/api/auth/change-password", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            auto body = json::parse(req.body);
            const std::string oldPassword = controller_utils::requiredJsonString(body, "oldPassword");
            const std::string newPassword = controller_utils::requiredJsonString(body, "newPassword");

            if (newPassword.size() < 6) {
                throw std::invalid_argument("Новый пароль должен содержать минимум 6 символов");
            }

            if (!authService.changePassword(auth.userId, oldPassword, newPassword)) {
                throw controller_utils::HttpError(400, "Текущий пароль указан неверно");
            }

            json response;
            response["success"] = true;
            response["message"] = "Password updated";
            res.set_content(response.dump(), "application/json");
        } catch(const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}

std::string AuthController::roleToString(UserRole role) {
    switch(role) {
        case UserRole::Student: return "Student";
        case UserRole::Teacher: return "Teacher";
        case UserRole::Admin: return "Admin";
    }
    return "Unknown";
}
