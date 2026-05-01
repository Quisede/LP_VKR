#pragma once

#include <string>
#include <vector>
#include "../models/Material.h"
#include "../repositories/MaterialRepository.h"

class MaterialService {
public:
    MaterialService(MaterialRepository& repo);

    std::vector<Material> getMaterialsForLesson(int lessonId);
    Material createMaterial(
        int lessonId,
        const std::string& title,
        const std::string& type,
        const std::string& content);

private:
    MaterialRepository& materialRepository;
};
