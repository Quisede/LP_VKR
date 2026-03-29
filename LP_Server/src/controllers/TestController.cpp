#include "TestController.h"
#include "CourseController.h"
#include "json.hpp"

using json = nlohmann::json;

static UserRole roleFromString(const std::string& role) {
    if(role == "Student") return UserRole::Student;
    if(role == "Teacher") return UserRole::Teacher;
    
    return UserRole::Admin;
}

TestController::TestController(TestService& service)
    : testService(service) {}

void TestController::registerRoutes(httplib::Server& server) {

    server.Get(R"(/api/courses/(\d+)/tests)",
    [this](const httplib::Request& req, httplib::Response& res) {

        int courseId = std::stoi(req.matches[1]);
        int userId = std::stoi(req.get_header_value("X-User-ID"));
        UserRole role = roleFromString(req.get_header_value("X-User-Role"));

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
    });
}