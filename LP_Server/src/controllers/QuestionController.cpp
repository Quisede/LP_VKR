#include "QuestionController.h"
#include "ControllerUtils.h"
#include "json.hpp"

using json = nlohmann::json;

QuestionController::QuestionController(QuestionService& service, JwtService& jwtService)
    : questionService(service),
      jwtService(jwtService) {}

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

    server.Get(R"(/api/tests/(\d+)/questions/manage)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int testId = controller_utils::pathParamInt(req, 1, "testId");
            auto auth = controller_utils::requireAuth(req, jwtService);

            auto questions = questionService.getQuestionsForManagement(auth.userId, auth.role, testId);

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
                    {"options", options},
                    {"correctAnswerId", q.correctAnswerId}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post(R"(/api/tests/(\d+)/questions)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int testId = controller_utils::pathParamInt(req, 1, "testId");
            auto auth = controller_utils::requireAuth(req, jwtService);

            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can create questions");
            }

            auto body = json::parse(req.body);
            std::string text = controller_utils::requiredJsonString(body, "text");
            int correctOptionIndex = controller_utils::requiredJsonInt(body, "correctOptionIndex");

            std::vector<std::string> options;
            for (const auto& item : controller_utils::requiredJsonArray(body, "options")) {
                if (!item.is_string()) {
                    throw std::invalid_argument("Each answer option must be a string");
                }

                options.push_back(item.get<std::string>());
            }

            Question question = questionService.createQuestion(
                auth.userId,
                auth.role,
                testId,
                text,
                options,
                correctOptionIndex);

            json response;
            response["id"] = question.id;
            response["testId"] = question.testId;
            response["text"] = question.text;
            response["correctAnswerId"] = question.correctAnswerId;
            response["options"] = json::array();

            for (const auto& option : question.options) {
                response["options"].push_back({
                    {"id", option.id},
                    {"text", option.text}
                });
            }

            res.status = 201;
            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Put(R"(/api/questions/(\d+))",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int questionId = controller_utils::pathParamInt(req, 1, "questionId");
            auto auth = controller_utils::requireAuth(req, jwtService);

            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can update questions");
            }

            auto body = json::parse(req.body);
            std::string text = controller_utils::requiredJsonString(body, "text");
            int correctOptionIndex = controller_utils::requiredJsonInt(body, "correctOptionIndex");

            std::vector<std::string> options;
            for (const auto& item : controller_utils::requiredJsonArray(body, "options")) {
                if (!item.is_string()) {
                    throw std::invalid_argument("Each answer option must be a string");
                }
                options.push_back(item.get<std::string>());
            }

            Question question = questionService.updateQuestion(
                auth.userId,
                auth.role,
                questionId,
                text,
                options,
                correctOptionIndex);

            json response;
            response["id"] = question.id;
            response["testId"] = question.testId;
            response["text"] = question.text;
            response["correctAnswerId"] = question.correctAnswerId;
            response["options"] = json::array();

            for (const auto& option : question.options) {
                response["options"].push_back({
                    {"id", option.id},
                    {"text", option.text}
                });
            }

            res.set_content(response.dump(), "application/json");
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Delete(R"(/api/questions/(\d+))",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int questionId = controller_utils::pathParamInt(req, 1, "questionId");
            auto auth = controller_utils::requireAuth(req, jwtService);

            if (auth.role != UserRole::Teacher && auth.role != UserRole::Admin) {
                throw controller_utils::HttpError(403, "Only teachers can delete questions");
            }

            questionService.deleteQuestion(auth.userId, auth.role, questionId);
            res.status = 204;
        } catch (const std::exception& ex) {
            controller_utils::handleRouteException(res, ex);
        }
    });

    server.Post(R"(/api/tests/(\d+)/submit)",
    [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int testId = controller_utils::pathParamInt(req, 1, "testId");
            auto auth = controller_utils::requireAuth(req, jwtService);

            auto body = json::parse(req.body);

            std::vector<SubmittedAnswer> answers;

            for (const auto& item : controller_utils::requiredJsonArray(body, "answers")) {
                answers.push_back({
                    controller_utils::requiredJsonInt(item, "questionId"),
                    controller_utils::requiredJsonInt(item, "answerId")
                });
            }

            auto result = questionService.submitTest(auth.userId, testId, answers);

            json response;
            response["testId"] = testId;
            response["courseId"] = result.courseId;
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
