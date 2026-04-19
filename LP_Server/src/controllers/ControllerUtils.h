#pragma once

#include "httplib.h"
#include "json.hpp"
#include "../models/User.h"
#include <stdexcept>
#include <string>

namespace controller_utils {

// Исключение для ошибок, где контроллер уже знает нужный HTTP-статус.
class HttpError : public std::runtime_error {
public:
    HttpError(int status, const std::string& message)
        : std::runtime_error(message), statusCode(status) {}

    int status() const {
        return statusCode;
    }

private:
    int statusCode;
};

// Отправляет ошибки в едином JSON-формате: {"error":"..."}.
inline void sendJsonError(httplib::Response& res, int status, const std::string& message) {
    nlohmann::json response;
    response["error"] = message;
    res.status = status;
    res.set_content(response.dump(), "application/json");
}

// Строго парсит целые числа: "12" корректно, "12abc" уже ошибка.
inline int parseInt(const std::string& value, const std::string& fieldName) {
    try {
        size_t parsed = 0;
        int result = std::stoi(value, &parsed);
        if (parsed != value.size()) {
            throw std::invalid_argument("trailing characters");
        }
        return result;
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid integer value for " + fieldName);
    }
}

// Читает параметр из regex-маршрута, например /api/courses/(\d+) -> req.matches[1].
inline int pathParamInt(const httplib::Request& req, size_t index, const std::string& fieldName) {
    if (req.matches.size() <= index) {
        throw std::invalid_argument("Missing path parameter: " + fieldName);
    }
    return parseInt(req.matches[index], fieldName);
}

// Проверяет наличие поля в JSON и возвращает его значение.
inline const nlohmann::json& requiredJsonField(const nlohmann::json& body, const std::string& fieldName) {
    if (!body.is_object()) {
        throw std::invalid_argument("JSON body must be an object");
    }

    auto it = body.find(fieldName);
    if (it == body.end()) {
        throw std::invalid_argument("Missing JSON field: " + fieldName);
    }

    return *it;
}

// Читает обязательную строку из JSON.
inline std::string requiredJsonString(const nlohmann::json& body, const std::string& fieldName) {
    const auto& value = requiredJsonField(body, fieldName);
    if (!value.is_string()) {
        throw std::invalid_argument("JSON field must be a string: " + fieldName);
    }

    return value.get<std::string>();
}

// Читает обязательное целое число из JSON.
inline int requiredJsonInt(const nlohmann::json& body, const std::string& fieldName) {
    const auto& value = requiredJsonField(body, fieldName);
    if (!value.is_number_integer()) {
        throw std::invalid_argument("JSON field must be an integer: " + fieldName);
    }

    return value.get<int>();
}

// Читает обязательный массив из JSON.
inline const nlohmann::json& requiredJsonArray(const nlohmann::json& body, const std::string& fieldName) {
    const auto& value = requiredJsonField(body, fieldName);
    if (!value.is_array()) {
        throw std::invalid_argument("JSON field must be an array: " + fieldName);
    }

    return value;
}

// Читает обязательный HTTP-заголовок и возвращает заданный статус, если его нет.
inline std::string requiredHeader(const httplib::Request& req, const std::string& name, int status = 400) {
    if (!req.has_header(name)) {
        throw HttpError(status, "Missing required header: " + name);
    }
    return req.get_header_value(name);
}

// Читает обязательный числовой заголовок. Отсутствующий X-User-Id считается 401.
inline int requiredIntHeader(const httplib::Request& req, const std::string& name) {
    int status = name == "X-User-Id" ? 401 : 400;
    return parseInt(requiredHeader(req, name, status), name);
}

// Переводит строковую роль во внутренний enum без опасного fallback в Admin.
inline UserRole parseUserRole(const std::string& role) {
    if (role == "Student") return UserRole::Student;
    if (role == "Teacher") return UserRole::Teacher;
    if (role == "Admin") return UserRole::Admin;
    throw std::invalid_argument("Invalid X-User-Role");
}

// Читает X-User-Role и отклоняет отсутствующие/неизвестные роли.
inline UserRole requiredUserRole(const httplib::Request& req) {
    return parseUserRole(requiredHeader(req, "X-User-Role", 401));
}

// Общая защита для роутов: ошибки валидации -> 400, неожиданные ошибки -> 500.
inline void handleRouteException(httplib::Response& res, const std::exception& ex) {
    if (const auto* httpError = dynamic_cast<const HttpError*>(&ex)) {
        sendJsonError(res, httpError->status(), httpError->what());
        return;
    }

    if (dynamic_cast<const nlohmann::json::exception*>(&ex) != nullptr ||
        dynamic_cast<const std::invalid_argument*>(&ex) != nullptr ||
        dynamic_cast<const std::out_of_range*>(&ex) != nullptr) {
        sendJsonError(res, 400, ex.what());
        return;
    }

    sendJsonError(res, 500, "Internal server error");
}

}
