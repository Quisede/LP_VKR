#pragma once

#include <vector>
#include "../models/Material.h"
#include "../repositories/MaterialRepository.h"

class MaterialService {
public:
    MaterialService(MaterialRepository& repo);

    std::vector<Material> getMaterialsForLesson(int lessonId);

private:
    MaterialRepository& materialRepository;
};