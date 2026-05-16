#include "LessonController.h"
#include "ControllerUtils.h"
#include "json.hpp"
#include <optional>

using json = nlohmann::json;

LessonController::LessonController(
    LessonService& service,
    CourseService& courseService,
    JwtService& jwtService)
    : lessonService(service),
      courseService(courseService),
      jwtService(jwtService) {}

void LessonController::registerRoutes(httplib::Server& server) {
    server.Get(R"(/api/courses/(\d+)/lessons)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int courseId = controller_utils::pathParamInt(req, 1, "courseId");
            std::optional<controller_utils::AuthContext> auth;
            try {
                auth = controller_utils::requireAuth(req, jwtService);
            } catch (const std::exception&) {
                auth = std::nullopt;
            }

            auto lessons = auth.has_value() && auth->role == UserRole::Student
                ? lessonService.getLessonsForCourseForUser(courseId, auth->userId)
                : lessonService.getLessonsForCourse(courseId);

            json response;
            response["lessons"] = json::array();

            for (const auto& lesson : lessons) {
                response["lessons"].push_back({
                    {"id", lesson.id},
                    {"courseId", lesson.courseId},
                    {"title", lesson.title},
                    {"content", lesson.content},
                    {"completed", lesson.completed}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post(R"(/api/lessons/(\d+)/complete)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Student) {
                throw controller_utils::HttpError(403, "Only students can mark lessons as completed");
            }

            const int lessonId = controller_utils::pathParamInt(req, 1, "lessonId");
            const auto lesson = lessonService.getLessonById(lessonId);
            if (!lesson.has_value()) {
                throw controller_utils::HttpError(404, "Lesson not found");
            }

            lessonService.markLessonCompleted(lessonId, auth.userId);
            res.set_content(json{
                {"lessonId", lessonId},
                {"completed", true}
            }.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post(R"(/api/courses/(\d+)/lessons)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can create lessons");
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
            std::string content = controller_utils::requiredJsonString(body, "content");

            Lesson lesson = lessonService.createLesson(courseId, title, content);

            json response{
                {"id", lesson.id},
                {"courseId", lesson.courseId},
                {"title", lesson.title},
                {"content", lesson.content}
            };

            res.status = 201;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Put(R"(/api/lessons/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can update lessons");
            }

            int lessonId = controller_utils::pathParamInt(req, 1, "lessonId");
            auto lesson = lessonService.getLessonById(lessonId);
            if (!lesson.has_value()) {
                throw controller_utils::HttpError(404, "Lesson not found");
            }

            if (!courseService.canManageCourse(auth.userId, auth.role, lesson->courseId)) {
                throw controller_utils::HttpError(403, "You can manage only your own courses");
            }

            json body = json::parse(req.body);
            std::string title = controller_utils::requiredJsonString(body, "title");
            std::string content = controller_utils::requiredJsonString(body, "content");

            Lesson updated = lessonService.updateLesson(lessonId, title, content);

            res.set_content(json{
                {"id", updated.id},
                {"courseId", updated.courseId},
                {"title", updated.title},
                {"content", updated.content}
            }.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Delete(R"(/api/lessons/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can delete lessons");
            }

            int lessonId = controller_utils::pathParamInt(req, 1, "lessonId");
            auto lesson = lessonService.getLessonById(lessonId);
            if (!lesson.has_value()) {
                throw controller_utils::HttpError(404, "Lesson not found");
            }

            if (!courseService.canManageCourse(auth.userId, auth.role, lesson->courseId)) {
                throw controller_utils::HttpError(403, "You can manage only your own courses");
            }

            lessonService.deleteLesson(lessonId);
            res.status = 204;
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}
