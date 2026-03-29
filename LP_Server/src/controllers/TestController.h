#pragma once

#include "httplib.h"
#include "../services/TestService.h"

class TestController {
public:
    TestController(TestService& service);

    void registerRoutes(httplib::Server& server);

private:
    TestService& testService;
};