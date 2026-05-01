#pragma once

#include "httplib.h"
#include "../services/TestService.h"
#include "../services/CourseService.h"
#include "../services/JwtService.h"

class TestController {
public:
    TestController(TestService& service, CourseService& courseService, JwtService& jwtService);

    void registerRoutes(httplib::Server& server);

private:
    TestService& testService;
    CourseService& courseService;
    JwtService& jwtService;
};
