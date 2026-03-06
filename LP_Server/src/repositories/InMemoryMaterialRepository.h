#pragma once

#include "MaterialRepository.h"
#include <vector>

class InMemoryMaterialRepository : public MaterialRepository {
public:
    InMemoryMaterialRepository();

    std::vector<Material> getMaterialsForLesson(int lessonId) override;

private:
    std::vector<Material> materials;
};