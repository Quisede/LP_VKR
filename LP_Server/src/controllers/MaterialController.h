#pragma once

#include "httplib.h"
#include "../services/MaterialService.h"
#include "../services/LessonService.h"
#include "../services/CourseService.h"
#include "../services/JwtService.h"

class MaterialController {
public:
    MaterialController(
        MaterialService& service,
        LessonService& lessonService,
        CourseService& courseService,
        JwtService& jwtService);

    void registerRoutes(httplib::Server& server);

private:
    MaterialService& materialService;
    LessonService& lessonService;
    CourseService& courseService;
    JwtService& jwtService;
};
