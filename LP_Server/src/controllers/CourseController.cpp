//
//  CourseController.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#include "CourseController.h"
#include "json.hpp"

using json = nlohmann::json;

CourseController::CourseController(CourseService& service):
    courseService(service) {}

static UserRole roleFromString(const std::string& role) {
    if(role == "Student") return UserRole::Student;
    if(role == "Teacher") return UserRole::Teacher;
    
    return UserRole::Admin;
}

void CourseController::registerRoutes(httplib::Server &server) {
    /*
     @param this - захват текущий оюъект в лямбда функцию
     @param req - входящий http запрос
     @param res - исходящий http ответ ( в него записываем результат)
     */
    
    /* Регистрируем GET запрос на путь /api/courses */
    server.Get("/api/courses", [this](const httplib::Request& req, httplib::Response& res) {
        
        /* проверяем наличие кастомных заголовков */
        if(!req.has_header("X-User-Id") || !req.has_header("X-User-Role")) {
            /* если заголовков нет то возвращаем ошибку */
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }
        
        int userId = std::stoi(req.get_header_value("X-User-Id"));
        UserRole role = roleFromString(req.get_header_value("X-User-Role"));
        
        auto courses = courseService.getCoursesForUser(userId, role);
        
        /* создаем объект JSON */
        json response;
        /* создает массив courses в этом объекте */
        response["courses"] = json::array();
        /* для каждого курса добавляем объект с полями */
        
        for(const auto& c : courses) {
            response["courses"].push_back({
                {"id", c.id},
                {"title", c.title},
                {"description", c.description},
                {"teacherId", c.teacherId},
            });
        }
        
        res.set_content(response.dump(), "application/json");
    });

    server.Post(R"(/api/courses/(\d+)/enroll)", [this](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_header("X-User-Id") || !req.has_header("X-User-Role")) {
        // если заголовков нет, то возвращаем ошибку
        res.status = 401;
        res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
        return;
    }

    int userId = std::stoi(req.get_header_value("X-User-Id"));
    UserRole role = roleFromString(
        req.get_header_value("X-User-Role")
    );

    int courseId = std::stoi(req.matches[1]);

    bool success = courseService.enrollStudent(
        userId, role, courseId
    );

    json response;

    if (success) {
        response["success"] = true;
    } else {
        response["success"] = false;
        response["error"] = "Enrollment failed";
    }

    res.set_content(response.dump(), "application/json");
    });
}
