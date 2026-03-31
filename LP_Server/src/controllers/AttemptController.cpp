#include "AttemptController.h"
#include "CourseController.h"
#include "json.hpp"

using json = nlohmann::json;

AttemptController::AttemptController(AttemptService& service)
    : attemptService(service) {}

static UserRole roleFromString(const std::string& role) {
    if(role == "Student") return UserRole::Student;
    if(role == "Teacher") return UserRole::Teacher;
    
    return UserRole::Admin;
}

void AttemptController::registerRoutes(httplib::Server& server) {

    server.Get(R"(/api/users/(\d+)/attempts)",
    [this](const httplib::Request& req, httplib::Response& res) {

        int requestedUserId = std::stoi(req.matches[1]);

        int currentUserId =
            std::stoi(req.get_header_value("X-User-Id"));

        UserRole role = roleFromString(
            req.get_header_value("X-User-Role"));

        auto attempts = attemptService.getAttemptsForUser(
            currentUserId,
            role,
            requestedUserId
        );

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