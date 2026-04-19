#pragma once

#include "MaterialRepository.h"
#include "../database/PostgresConnection.h"
#include "../models/Material.h"
#include <vector>

class PostgresMaterialRepository : public MaterialRepository {
public:
    PostgresMaterialRepository(PostgresConnection& connection);

    std::vector<Material> getMaterialsForLesson(int lessonId) override;

private:
    PostgresConnection& db;
};