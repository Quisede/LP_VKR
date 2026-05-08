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

CourseController::CourseController(CourseService& service, JwtService& jwtService):
    courseService(service),
    jwtService(jwtService) {}

void CourseController::registerRoutes(httplib::Server &server) {
    /*
     @param this - захват текущий оюъект в лямбда функцию
     @param req - входящий http запрос
     @param res - исходящий http ответ ( в него записываем результат)
     */
    
    /* Регистрируем GET запрос на путь /api/courses */
    server.Get("/api/courses", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            int page = controller_utils::optionalQueryInt(req, "page", 1, 1, 100000);
            int limit = controller_utils::optionalQueryInt(req, "limit", 10, 1, 100);
            
            auto allCourses = courseService.getCoursesForUser(auth.userId, auth.role);
            auto courses = courseService.getCoursesPaged(auth.userId, auth.role, page, limit);

            int total = static_cast<int>(allCourses.size());
            int totalPages = total == 0 ? 0 : (total + limit - 1) / limit;
            
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

            response["pagination"] = {
                {"page", page},
                {"limit", limit},
                {"total", total},
                {"totalPages", totalPages}
            };
            
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Get("/api/courses/all", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            controller_utils::requireAuth(req, jwtService);

            auto courses = courseService.getAllCourses();

            json response;
            response["courses"] = json::array();

            for (const auto& c : courses) {
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
            auto auth = controller_utils::requireAuth(req, jwtService);
            int courseId = controller_utils::pathParamInt(req, 1, "courseId");

            EnrollmentResult result = courseService.enrollStudent(
                auth.userId, auth.role, courseId
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

    server.Get(R"(/api/courses/(\d+)/students)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            int courseId = controller_utils::pathParamInt(req, 1, "courseId");

            auto students = courseService.getStudentsForCourse(auth.userId, auth.role, courseId);

            json response;
            response["students"] = json::array();

            for (const auto& student : students) {
                response["students"].push_back({
                    {"id", student.id},
                    {"login", student.login},
                    {"progress", student.progress}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post("/api/courses", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can create courses");
            }

            json body = json::parse(req.body);
            std::string title = controller_utils::requiredJsonString(body, "title");
            std::string description = controller_utils::requiredJsonString(body, "description");

            Course course = courseService.createCourse(auth.userId, title, description);

            json response{
                {"id", course.id},
                {"title", course.title},
                {"description", course.description},
                {"teacherId", course.teacherId}
            };

            res.status = 201;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Put(R"(/api/courses/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can update courses");
            }

            int courseId = controller_utils::pathParamInt(req, 1, "courseId");
            if (!courseService.getCourseById(courseId).has_value()) {
                throw controller_utils::HttpError(404, "Course not found");
            }

            if (!courseService.canManageCourse(auth.userId, auth.role, courseId)) {
                throw controller_utils::HttpError(403, "You can manage only your own courses");
            }

            json body = json::parse(req.body);
            std::string title = controller_utils::requiredJsonString(body, "title");
            std::string description = controller_utils::requiredJsonString(body, "description");

            Course course = courseService.updateCourse(courseId, title, description);

            res.set_content(json{
                {"id", course.id},
                {"title", course.title},
                {"description", course.description},
                {"teacherId", course.teacherId}
            }.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Delete(R"(/api/courses/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can delete courses");
            }

            int courseId = controller_utils::pathParamInt(req, 1, "courseId");
            if (!courseService.getCourseById(courseId).has_value()) {
                throw controller_utils::HttpError(404, "Course not found");
            }

            if (!courseService.canManageCourse(auth.userId, auth.role, courseId)) {
                throw controller_utils::HttpError(403, "You can manage only your own courses");
            }

            courseService.deleteCourse(courseId);
            res.status = 204;
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}
