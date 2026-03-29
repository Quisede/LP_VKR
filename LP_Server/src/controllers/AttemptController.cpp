#include "AttemptController.h"
#include "CourseController.h"
#include "json.hpp"

using json = nlohmann::json;

AttemptController::AttemptController(AttemptService& service)
    : attemptService(service) {}

void AttemptController::registerRoutes(httplib::Server& server) {

    server.Get(R"(/api/users/(\d+)/attempts)",
    [this](const httplib::Request& req, httplib::Response& res) {

        int userId = std::stoi(req.matches[1]);

        auto attempts = attemptService.getAttemptsForUser(userId);

        json response;
        response["attempts"] = json::array();

        for (const auto& a : attempts) {
            response["attempts"].push_back({
                {"testId", a.testId},
                {"score", a.score},
                {"total", a.total},
                {"percentage", a.percentage},
                {"passed", a.passed}
            });
        }

        res.set_content(response.dump(), "application/json");
    });
}