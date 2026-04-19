//
//  CourseController.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#include "CourseController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

CourseController::CourseController(CourseService& service):
    courseService(service) {}

void CourseController::registerRoutes(httplib::Server &server) {
    /*
     @param this - захват текущий оюъект в лямбда функцию
     @param req - входящий http запрос
     @param res - исходящий http ответ ( в него записываем результат)
     */
    
    /* Регистрируем GET запрос на путь /api/courses */
    server.Get("/api/courses", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int userId = controller_utils::requiredIntHeader(req, "X-User-Id");
            UserRole role = controller_utils::requiredUserRole(req);
            
            auto courses = courseService.getCoursesForUser(userId, role);
            
            json response;
            response["courses"] = json::array();
            
            for(const auto& c : courses) {
                response["courses"].push_back({
                    {"id", c.id},
                    {"title", c.title},
                    {"description", c.description},
                    {"teacherId", c.teacherId},
                });
            }
            
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post(R"(/api/courses/(\d+)/enroll)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int userId = controller_utils::requiredIntHeader(req, "X-User-Id");
            UserRole role = controller_utils::requiredUserRole(req);
            int courseId = controller_utils::pathParamInt(req, 1, "courseId");

            EnrollmentResult result = courseService.enrollStudent(
                userId, role, courseId
            );

            json response;
            response["success"] = result.success();

            if (result.success()) {
                response["message"] = result.message;
                res.set_content(response.dump(), "application/json");
                return;
            }

            response["error"] = result.message;

            if (result.status == EnrollmentStatus::ForbiddenRole) {
                res.status = 403;
            } else if (result.status == EnrollmentStatus::CourseNotFound) {
                res.status = 404;
            } else if (result.status == EnrollmentStatus::AlreadyEnrolled) {
                res.status = 409;
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}
