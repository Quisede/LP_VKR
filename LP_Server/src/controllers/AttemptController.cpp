#include "AttemptController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

AttemptController::AttemptController(AttemptService& service, JwtService& jwtService)
    : attemptService(service),
      jwtService(jwtService) {}

void AttemptController::registerRoutes(httplib::Server& server) {

    server.Get(R"(/api/users/(\d+)/attempts)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int requestedUserId = controller_utils::pathParamInt(req, 1, "userId");
            auto auth = controller_utils::requireAuth(req, jwtService);

            auto attempts = attemptService.getAttemptsForUser(
                auth.userId,
                auth.role,
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
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Get(R"(/api/courses/(\d+)/analytics)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int courseId = controller_utils::pathParamInt(req, 1, "courseId");
            auto auth = controller_utils::requireAuth(req, jwtService);

            auto analytics = attemptService.getCourseAnalytics(
                auth.userId,
                auth.role,
                courseId
            );

            json response;
            response["studentsCount"] = analytics.studentsCount;
            response["attemptsCount"] = analytics.attemptsCount;
            response["averagePercentage"] = analytics.averagePercentage;
            response["results"] = json::array();

            for (const auto& row : analytics.rows) {
                response["results"].push_back({
                    {"studentLogin", row.studentLogin},
                    {"testTitle", row.testTitle},
                    {"score", row.score},
                    {"total", row.total},
                    {"percentage", row.percentage},
                    {"passed", row.passed}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}
