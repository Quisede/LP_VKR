//
//  CourseController.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include "httplib.h"
#include "../services/CourseService.h"

class CourseController {
public:
    CourseController(CourseService& service);
    
    void registerRoutes(httplib::Server& server);
    
private:
    CourseService& courseService;
};
