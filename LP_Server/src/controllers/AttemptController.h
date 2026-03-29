#pragma once

#include "httplib.h"
#include "../services/AttemptService.h"

class AttemptController {
public:
    AttemptController(AttemptService& service);

    void registerRoutes(httplib::Server& server);

private:
    AttemptService& attemptService;
};