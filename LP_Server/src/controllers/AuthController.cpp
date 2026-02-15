//
//  AuthController.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 09.01.2026.
//

#include "AuthController.h"
#include "json.hpp"

using json = nlohmann::json;

AuthController::AuthController(AuthService& authService):
    authService(authService) {}

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
            
            std::string login = body["login"];
            std::string password = body["password"];
            
            /* возвращаем структуру с результатом аутентификации */
            AuthResult result = authService.login(login, password);
            
            /* создаем json-объект для ответа */
            json responce;
            
            /* формирование json-ответа в зависимости от успешности аутентификации */
            if(result.success) {
                responce["success"] = true;
                responce["userId"] = result.userId;
                responce["role"] = roleToString(result.role);
            } else {
                responce["success"] = false;
                responce["error"] = result.errorMessage;
            }
            
            /* ответ клиенту */
            res.set_content(responce.dump(), "application/json");
        } catch(...) {
            /* устанавливаем HTTP статус "Bad Request" (400) */
            res.status = 400;
            /* Отправляем JSON с сообщением об ошибке */
            res.set_content("{\"error\":\"Invalid JSON\"}", "application/json");
        }
    });
    
    // регистрация
    server.Post("/api/auth/register", [this](const httplib::Request& req, httplib::Response& res) {
        
        try {
            /* парсим json-тело запроса */
            auto body = json::parse(req.body);
            
            std::string login = body["login"];
            std::string password = body["password"];
            
            /* возвращаем структуру с результатом аутентификации */
            AuthResult result = authService.registerUser(login, password, UserRole::Student);
            
            /* создаем json-объект для ответа */
            json responce;
            
            /* формирование json-ответа в зависимости от успешности аутентификации */
            if(result.success) {
                responce["success"] = true;
                responce["userId"] = result.userId;
                responce["role"] = "Student"; // ставим студента по дефолт, преподам аккаунты будет создавать админ
            } else {
                responce["success"] = false;
                responce["error"] = result.errorMessage;
            }
            
            /* ответ клиенту */
            res.set_content(responce.dump(), "application/json");
        } catch(...) {
            /* устанавливаем HTTP статус "Bad Request" (400) */
            res.status = 400;
            /* Отправляем JSON с сообщением об ошибке */
            res.set_content("{\"error\":\"Invalid JSON\"}", "application/json");
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
