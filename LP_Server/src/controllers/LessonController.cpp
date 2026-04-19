#include "LessonController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

LessonController::LessonController(LessonService& service)
    : lessonService(service) {}

void LessonController::registerRoutes(httplib::Server& server) {
    server.Get(R"(/api/courses/(\d+)/lessons)",[this](const httplib::Request& req, httplib::Response& res) {
        try {
            int courseId = controller_utils::pathParamInt(req, 1, "courseId");

            auto lessons = lessonService.getLessonsForCourse(courseId);

            json response;
            response["lessons"] = json::array();

            for(const auto& lesson : lessons) {
                response["lessons"].push_back({
                    {"id", lesson.id},
                    {"title", lesson.title},
                    {"content", lesson.content}
                });
            }
            
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
    
}
