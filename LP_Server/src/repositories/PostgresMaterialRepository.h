#pragma once

#include "MaterialRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Material.h"
#include <optional>
#include <vector>

class PostgresMaterialRepository : public MaterialRepository {
public:
    PostgresMaterialRepository(PostgresConnection& connection);

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
    PostgresConnection& db;
};
