#include "TestController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

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

                Test test = testService.createTest(courseId, title);

                json response{
                    {"id", test.id},
                    {"courseId", test.courseId},
                    {"title", test.title}
                };

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

                Test updated = testService.updateTest(testId, title);

                json response{
                    {"id", updated.id},
                    {"courseId", updated.courseId},
                    {"title", updated.title}
                };

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
