#include "MaterialController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

MaterialController::MaterialController(MaterialService& service)
    :materialService(service) {}

void MaterialController::registerRoutes(httplib::Server& server) {
    server.Get(R"(/api/lessons/(\d+)/materials)",[this](const httplib::Request& req, httplib::Response& res) {
        try {
            int lessonId = controller_utils::pathParamInt(req, 1, "lessonId");

            auto materials = materialService.getMaterialsForLesson(lessonId);

            json response;
            response["materials"] = json::array();

            for(const auto& material : materials) {
                response["materials"].push_back({
                    {"id", material.id},
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
}
