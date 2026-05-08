#pragma once

#include "MaterialRepository.h"
#include <vector>

class InMemoryMaterialRepository : public MaterialRepository {
public:
    InMemoryMaterialRepository();

    std::vector<Material> getMaterialsForLesson(int lessonId) override;
    std::optional<Material> getMaterialById(int materialId) override;
    Material createMaterial(
        int lessonId,
        const std::string& title,
        const std::string& type,
        const std::string& content) override;
    Material updateMaterial(
        int materialId,
        const std::string& title,
        const std::string& type,
        const std::string& content) override;
    void deleteMaterial(int materialId) override;

private:
    std::vector<Material> materials;
};
