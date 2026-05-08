#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../models/Material.h"
#include "../repositories/MaterialRepository.h"

class MaterialService {
public:
    MaterialService(MaterialRepository& repo);

    std::vector<Material> getMaterialsForLesson(int lessonId);
    std::optional<Material> getMaterialById(int materialId);
    Material createMaterial(
        int lessonId,
        const std::string& title,
        const std::string& type,
        const std::string& content);
    Material updateMaterial(
        int materialId,
        const std::string& title,
        const std::string& type,
        const std::string& content);
    void deleteMaterial(int materialId);

private:
    MaterialRepository& materialRepository;
};
