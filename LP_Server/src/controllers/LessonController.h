#pragma once

#include "httplib.h"
#include "../services/LessonService.h"

class LessonController {
public:
    LessonController(LessonService& service);

    void registerRoutes(httplib::Server& server);
private:
    LessonService& lessonService;
};