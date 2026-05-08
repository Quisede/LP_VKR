#include "MaterialController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

MaterialController::MaterialController(
    MaterialService& service,
    LessonService& lessonService,
    CourseService& courseService,
    JwtService& jwtService)
    : materialService(service),
      lessonService(lessonService),
      courseService(courseService),
      jwtService(jwtService) {}

void MaterialController::registerRoutes(httplib::Server& server) {
    server.Get(R"(/api/lessons/(\d+)/materials)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int lessonId = controller_utils::pathParamInt(req, 1, "lessonId");

            auto materials = materialService.getMaterialsForLesson(lessonId);

            json response;
            response["materials"] = json::array();

            for (const auto& material : materials) {
                response["materials"].push_back({
                    {"id", material.id},
                    {"lessonId", material.lessonId},
                    {"title", material.title},
                    {"type", material.type},
                    {"content", material.content}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post(R"(/api/lessons/(\d+)/materials)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can create materials");
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
            std::string type = controller_utils::requiredJsonString(body, "type");
            std::string content = controller_utils::requiredJsonString(body, "content");

            Material material = materialService.createMaterial(lessonId, title, type, content);

            json response{
                {"id", material.id},
                {"lessonId", material.lessonId},
                {"title", material.title},
                {"type", material.type},
                {"content", material.content}
            };

            res.status = 201;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Put(R"(/api/materials/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can update materials");
            }

            int materialId = controller_utils::pathParamInt(req, 1, "materialId");
            auto material = materialService.getMaterialById(materialId);
            if (!material.has_value()) {
                throw controller_utils::HttpError(404, "Material not found");
            }

            auto lesson = lessonService.getLessonById(material->lessonId);
            if (!lesson.has_value()) {
                throw controller_utils::HttpError(404, "Lesson not found");
            }

            if (!courseService.canManageCourse(auth.userId, auth.role, lesson->courseId)) {
                throw controller_utils::HttpError(403, "You can manage only your own courses");
            }

            json body = json::parse(req.body);
            std::string title = controller_utils::requiredJsonString(body, "title");
            std::string type = controller_utils::requiredJsonString(body, "type");
            std::string content = controller_utils::requiredJsonString(body, "content");

            Material updated = materialService.updateMaterial(materialId, title, type, content);

            res.set_content(json{
                {"id", updated.id},
                {"lessonId", updated.lessonId},
                {"title", updated.title},
                {"type", updated.type},
                {"content", updated.content}
            }.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Delete(R"(/api/materials/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            auto auth = controller_utils::requireAuth(req, jwtService);
            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can delete materials");
            }

            int materialId = controller_utils::pathParamInt(req, 1, "materialId");
            auto material = materialService.getMaterialById(materialId);
            if (!material.has_value()) {
                throw controller_utils::HttpError(404, "Material not found");
            }

            auto lesson = lessonService.getLessonById(material->lessonId);
            if (!lesson.has_value()) {
                throw controller_utils::HttpError(404, "Lesson not found");
            }

            if (!courseService.canManageCourse(auth.userId, auth.role, lesson->courseId)) {
                throw controller_utils::HttpError(403, "You can manage only your own courses");
            }

            materialService.deleteMaterial(materialId);
            res.status = 204;
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}
