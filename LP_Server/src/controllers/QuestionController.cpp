#include "QuestionController.h"
#include "CourseController.h"
#include "json.hpp"

using json = nlohmann::json;

static UserRole roleFromString(const std::string& role) {
    if(role == "Student") return UserRole::Student;
    if(role == "Teacher") return UserRole::Teacher;
    
    return UserRole::Admin;
}

QuestionController::QuestionController(QuestionService& service)
    : questionService(service) {}

void QuestionController::registerRoutes(httplib::Server& server) {

    server.Get(R"(/api/tests/(\d+)/questions)",
    [this](const httplib::Request& req, httplib::Response& res) {

        int testId = std::stoi(req.matches[1]);

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

            // ❗ correctAnswerId НЕ отправляем
        }

        res.set_content(response.dump(), "application/json");
    });

    server.Post(R"(/api/tests/(\d+)/submit)",
    [this](const httplib::Request& req, httplib::Response& res) {

        int testId = std::stoi(req.matches[1]);

        auto body = json::parse(req.body);

        std::vector<SubmittedAnswer> answers;

        for (const auto& item : body["answers"]) {
            answers.push_back({
                item["questionId"],
                item["answerId"]
            });
        }

        int userId = std::stoi(req.get_header_value("X-User-Id"));

        auto result = questionService.submitTest(userId, testId, answers);

        json response;
        response["score"] = result.score;
        response["total"] = result.total;
        response["percentage"] = result.percentage;
        response["passed"] = result.passed;

        res.set_content(response.dump(), "application/json");
    });
}