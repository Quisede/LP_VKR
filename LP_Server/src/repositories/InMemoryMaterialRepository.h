#pragma once

#include "MaterialRepository.h"
#include <vector>

class InMemoryMaterialRepository : public MaterialRepository {
public:
    InMemoryMaterialRepository();

    std::vector<Material> getMaterialsForLesson(int lessonId) override;
    Material createMaterial(
        int lessonId,
        const std::string& title,
        const std::string& type,
        const std::string& content) override;

private:
    std::vector<Material> materials;
};
