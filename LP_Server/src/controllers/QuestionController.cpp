#include "QuestionController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

QuestionController::QuestionController(QuestionService& service)
    : questionService(service) {}

void QuestionController::registerRoutes(httplib::Server& server) {

    server.Get(R"(/api/tests/(\d+)/questions)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int testId = controller_utils::pathParamInt(req, 1, "testId");

            auto questions = questionService.getQuestionsForTest(testId);

            json response;
            response["questions"] = json::array();

            for (const auto& q : questions) {

                json options = json::array();

                for (const auto& opt : q.options) {
                    options.push_back({
                        {"id", opt.id},
                        {"text", opt.text}
                    });
                }

                response["questions"].push_back({
                    {"id", q.id},
                    {"text", q.text},
                    {"options", options}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post(R"(/api/tests/(\d+)/submit)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int testId = controller_utils::pathParamInt(req, 1, "testId");

            auto body = json::parse(req.body);

            std::vector<SubmittedAnswer> answers;

            for (const auto& item : controller_utils::requiredJsonArray(body, "answers")) {
                answers.push_back({
                    controller_utils::requiredJsonInt(item, "questionId"),
                    controller_utils::requiredJsonInt(item, "answerId")
                });
            }

            int userId = controller_utils::requiredIntHeader(req, "X-User-Id");

            auto result = questionService.submitTest(userId, testId, answers);

            json response;
            response["score"] = result.score;
            response["total"] = result.total;
            response["percentage"] = result.percentage;
            response["passed"] = result.passed;

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });
}
