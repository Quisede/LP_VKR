#pragma once

#include <vector>
#include "../models/Material.h"

class MaterialRepository {
public:
    virtual ~MaterialRepository() = default;

    virtual std::vector<Material> getMaterialsForLesson(int lessinId) = 0;
};