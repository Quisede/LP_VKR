#pragma once

#include "httplib.h"
#include "../services/MaterialService.h"

class MaterialController {
public:
    MaterialController(MaterialService& service);

    void registerRoutes(httplib::Server& server);

private:
    MaterialService& materialService;
};