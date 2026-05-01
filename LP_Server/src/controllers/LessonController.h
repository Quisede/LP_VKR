#pragma once

#include "httplib.h"
#include "../services/LessonService.h"
#include "../services/CourseService.h"
#include "../services/JwtService.h"

class LessonController {
public:
    LessonController(LessonService& service, CourseService& courseService, JwtService& jwtService);

    void registerRoutes(httplib::Server& server);
private:
    LessonService& lessonService;
    CourseService& courseService;
    JwtService& jwtService;
};
