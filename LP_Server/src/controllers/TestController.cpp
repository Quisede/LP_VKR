#include "TestController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

TestController::TestController(TestService& service)
    : testService(service) {}

void TestController::registerRoutes(httplib::Server& server) {

    server.Get(R"(/api/courses/(\d+)/tests)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int courseId = controller_utils::pathParamInt(req, 1, "courseId");
            int userId = controller_utils::requiredIntHeader(req, "X-User-Id");
            UserRole role = controller_utils::requiredUserRole(req);

            auto tests = testService.getTestsForCourse(userId, role, courseId);

            json response;
            response["tests"] = json::array();

            for (const auto& test : tests) {

                response["tests"].push_back({
                    {"id", test.id},
                    {"title", test.title}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}
