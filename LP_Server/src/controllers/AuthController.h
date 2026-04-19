//
//  AuthController.h
//  LP_Server
//
//  Created by Даниил Дружинин on 09.01.2026.
//

#pragma once

#include "../services/AuthService.h"
#include "../services/JwtService.h"
#include "httplib.h"

class AuthController {
public:
    AuthController(AuthService& authService, JwtService& jwtService);
    
    void registerRoutes(httplib::Server& server);
    
    std::string roleToString(UserRole role);
    
private:
    AuthService& authService;
    JwtService& jwtService;
};
