#include "TestController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

namespace {
json testToJson(const Test& test) {
    return {
        {"id", test.id},
        {"courseId", test.courseId},
        {"title", test.title},
        {"status", test.status},
        {"deadlineAt", test.deadlineAt},
        {"available", test.available},
        {"maxAttempts", test.maxAttempts},
        {"timeLimitMinutes", test.timeLimitMinutes},
        {"attemptsUsed", test.attemptsUsed},
        {"passed", test.passed},
        {"bestPercentage", test.bestPercentage},
        {"canAttempt", test.canAttempt}
    };
}

std::string optionalString(const json& body, const std::string& key, const std::string& fallback = "") {
    if (!body.contains(key) || body.at(key).is_null()) {
        return fallback;
    }

    if (!body.at(key).is_string()) {
        throw std::invalid_argument(key + " must be a string");
    }

    return body.at(key).get<std::string>();
}

int optionalInt(const json& body, const std::string& key, int fallback = 0) {
    if (!body.contains(key) || body.at(key).is_null()) {
        return fallback;
    }

    if (!body.at(key).is_number_integer()) {
        throw std::invalid_argument(key + " must be an integer");
    }

    return body.at(key).get<int>();
}
}

TestController::TestController(
    TestService& service,
    CourseService& courseService,
    JwtService& jwtService)
    : testService(service),
      courseService(courseService),
      jwtService(jwtService) {}

void TestController::registerRoutes(httplib::Server& server) {
    server.Get(R"(/api/courses/(\d+)/tests)",
        [this](const httplib::Request& req, httplib::Response& res) {
            try {
                int courseId = controller_utils::pathParamInt(req, 1, "courseId");
                auto auth = controller_utils::requireAuth(req, jwtService);

                auto tests = testService.getTestsForCourse(auth.userId, auth.role, courseId);

                json response;
                response["tests"] = json::array();

                for (const auto& test : tests) {
                    response["tests"].push_back(testToJson(test));
                }

                res.set_content(response.dump(), "application/json");
            } catch (const std::exception& ex) {
                controller_utils::handleRouteException(res, ex);
            }
        });

    server.Post(R"(/api/courses/(\d+)/tests)",
        [this](const httplib::Request& req, httplib::Response& res) {
            try {
                auto auth = controller_utils::requireAuth(req, jwtService);
                if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                    throw controller_utils::HttpError(403, "Only teachers can create tests");
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
                std::string status = optionalString(body, "status", "active");
                std::string deadlineAt = optionalString(body, "deadlineAt");
                int maxAttempts = optionalInt(body, "maxAttempts", 0);
                int timeLimitMinutes = optionalInt(body, "timeLimitMinutes", 30);

                Test test = testService.createTest(courseId, title, status, deadlineAt, maxAttempts, timeLimitMinutes);

                json response = testToJson(test);

                res.status = 201;
                res.set_content(response.dump(), "application/json");
            } catch (const std::exception& ex) {
                controller_utils::handleRouteException(res, ex);
            }
        });

    server.Put(R"(/api/tests/(\d+))",
        [this](const httplib::Request& req, httplib::Response& res) {
            try {
                auto auth = controller_utils::requireAuth(req, jwtService);
                if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                    throw controller_utils::HttpError(403, "Only teachers can update tests");
                }

                int testId = controller_utils::pathParamInt(req, 1, "testId");
                auto test = testService.getTestById(testId);
                if (!test.has_value()) {
                    throw controller_utils::HttpError(404, "Test not found");
                }

                if (!courseService.canManageCourse(auth.userId, auth.role, test->courseId)) {
                    throw controller_utils::HttpError(403, "You can manage only your own courses");
                }

                json body = json::parse(req.body);
                std::string title = controller_utils::requiredJsonString(body, "title");
                std::string status = optionalString(body, "status", test->status);
                std::string deadlineAt = optionalString(body, "deadlineAt", test->deadlineAt);
                int maxAttempts = optionalInt(body, "maxAttempts", test->maxAttempts);
                int timeLimitMinutes = optionalInt(body, "timeLimitMinutes", test->timeLimitMinutes);

                Test updated = testService.updateTest(testId, title, status, deadlineAt, maxAttempts, timeLimitMinutes);

                json response = testToJson(updated);

                res.set_content(response.dump(), "application/json");
            } catch (const std::exception& ex) {
                controller_utils::handleRouteException(res, ex);
            }
        });

    server.Delete(R"(/api/tests/(\d+))",
        [this](const httplib::Request& req, httplib::Response& res) {
            try {
                auto auth = controller_utils::requireAuth(req, jwtService);
                if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                    throw controller_utils::HttpError(403, "Only teachers can delete tests");
                }

                int testId = controller_utils::pathParamInt(req, 1, "testId");
                auto test = testService.getTestById(testId);
                if (!test.has_value()) {
                    throw controller_utils::HttpError(404, "Test not found");
                }

                if (!courseService.canManageCourse(auth.userId, auth.role, test->courseId)) {
                    throw controller_utils::HttpError(403, "You can manage only your own courses");
                }

                testService.deleteTest(testId);
                res.status = 204;
            } catch (const std::exception& ex) {
                controller_utils::handleRouteException(res, ex);
            }
        });
}
