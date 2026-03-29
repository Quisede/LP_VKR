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
}