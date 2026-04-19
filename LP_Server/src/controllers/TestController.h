#pragma once

#include "httplib.h"
#include "../services/TestService.h"
#include "../services/JwtService.h"

class TestController {
public:
    TestController(TestService& service, JwtService& jwtService);

    void registerRoutes(httplib::Server& server);

private:
    TestService& testService;
    JwtService& jwtService;
};
