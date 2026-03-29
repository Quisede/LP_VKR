#pragma once

#include "httplib.h"
#include "../services/QuestionService.h"

class QuestionController {
public:
    QuestionController(QuestionService& service);

    void registerRoutes(httplib::Server& server);

private:
    QuestionService& questionService;
};