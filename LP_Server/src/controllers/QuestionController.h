#pragma once

#include "httplib.h"
#include "../services/QuestionService.h"
#include "../services/JwtService.h"

class QuestionController {
public:
    QuestionController(QuestionService& service, JwtService& jwtService);

    void registerRoutes(httplib::Server& server);

private:
    QuestionService& questionService;
    JwtService& jwtService;
};
