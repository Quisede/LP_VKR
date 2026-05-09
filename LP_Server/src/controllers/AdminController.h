#pragma once

#include "httplib.h"

#include "../services/AuthService.h"
#include "../services/AdminService.h"
#include "../services/JwtService.h"

class AdminController {
public:
    AdminController(AdminService& adminService, AuthService& authService, JwtService& jwtService);

    void registerRoutes(httplib::Server& server);

private:
    AdminService& adminService;
    AuthService& authService;
    JwtService& jwtService;
};
