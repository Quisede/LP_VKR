#include "LessonController.h"
#include "json.hpp"

using json = nlohmann::json;

LessonController::LessonController(LessonService& service)
    : lessonService(service) {}

void LessonController::registerRoutes(httplib::Server& server) {
    server.Get(R"(/api/courses/(\d+)/lessons)",[this](const httplib::Request& req, httplib::Response& res) {
        int courseId = std::stoi(req.matches[1]);

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
    });
    
}