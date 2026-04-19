#pragma once

#include "httplib.h"
#include "../services/AttemptService.h"
#include "../services/JwtService.h"

class AttemptController {
public:
    AttemptController(AttemptService& service, JwtService& jwtService);

    void registerRoutes(httplib::Server& server);

private:
    AttemptService& attemptService;
    JwtService& jwtService;
};
