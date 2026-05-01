#pragma once

#include "MaterialRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Material.h"
#include <vector>

class PostgresMaterialRepository : public MaterialRepository {
public:
    PostgresMaterialRepository(PostgresConnection& connection);

    std::vector<Material> getMaterialsForLesson(int lessonId) override;
    Material createMaterial(
        int lessonId,
        const std::string& title,
        const std::string& type,
        const std::string& content) override;

private:
    PostgresConnection& db;
};
