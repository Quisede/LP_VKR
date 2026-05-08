#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../models/Material.h"

class MaterialRepository {
public:
    virtual ~MaterialRepository() = default;

    virtual std::vector<Material> getMaterialsForLesson(int lessinId) = 0;
    virtual std::optional<Material> getMaterialById(int materialId) = 0;
    virtual Material createMaterial(
        int lessonId,
        const std::string& title,
        const std::string& type,
        const std::string& content) = 0;
    virtual Material updateMaterial(
        int materialId,
        const std::string& title,
        const std::string& type,
        const std::string& content) = 0;
    virtual void deleteMaterial(int materialId) = 0;
};
