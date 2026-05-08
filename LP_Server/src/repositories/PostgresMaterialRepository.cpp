#include "PostgresMaterialRepository.h"
#include <libpq-fe.h>
#include <stdexcept>

PostgresMaterialRepository::PostgresMaterialRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Material> PostgresMaterialRepository::getMaterialsForLesson(int lessonId) {
    std::lock_guard<std::mutex> lock(db.mutex());
    std::vector<Material> materials;

    std::string query = "SELECT id, lesson_id, title, type, content FROM materials WHERE lesson_id = " + std::to_string(lessonId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get materials for lesson: " + error);
    }

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        Material material;

        material.id = std::stoi(PQgetvalue(res, i, 0));
        material.lessonId = std::stoi(PQgetvalue(res, i, 1));
        material.title = PQgetvalue(res, i, 2);
        material.type = PQgetvalue(res, i, 3);
        material.content = PQgetvalue(res, i, 4);
        
        materials.push_back(material);
    }

    PQclear(res);
    return materials;
}

std::optional<Material> PostgresMaterialRepository::getMaterialById(int materialId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string materialIdValue = std::to_string(materialId);
    const char* params[] = {materialIdValue.c_str()};

    PGresult* res = PQexecParams(
        db.get(),
        "SELECT id, lesson_id, title, type, content FROM materials WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to get material by id: " + error);
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return std::nullopt;
    }

    Material material;
    material.id = std::stoi(PQgetvalue(res, 0, 0));
    material.lessonId = std::stoi(PQgetvalue(res, 0, 1));
    material.title = PQgetvalue(res, 0, 2);
    material.type = PQgetvalue(res, 0, 3);
    material.content = PQgetvalue(res, 0, 4);
    PQclear(res);
    return material;
}

Material PostgresMaterialRepository::createMaterial(
    int lessonId,
    const std::string& title,
    const std::string& type,
    const std::string& content) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string lessonIdValue = std::to_string(lessonId);
    const char* params[] = {
        lessonIdValue.c_str(),
        title.c_str(),
        type.c_str(),
        content.c_str()
    };

    PGresult* res = PQexecParams(
        db.get(),
        "INSERT INTO materials (lesson_id, title, type, content) "
        "VALUES ($1, $2, $3, $4) "
        "RETURNING id, lesson_id, title, type, content",
        4,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to create material: " + error);
    }

    Material material;
    material.id = std::stoi(PQgetvalue(res, 0, 0));
    material.lessonId = std::stoi(PQgetvalue(res, 0, 1));
    material.title = PQgetvalue(res, 0, 2);
    material.type = PQgetvalue(res, 0, 3);
    material.content = PQgetvalue(res, 0, 4);
    PQclear(res);
    return material;
}

Material PostgresMaterialRepository::updateMaterial(
    int materialId,
    const std::string& title,
    const std::string& type,
    const std::string& content) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string materialIdValue = std::to_string(materialId);
    const char* params[] = {
        materialIdValue.c_str(),
        title.c_str(),
        type.c_str(),
        content.c_str()
    };

    PGresult* res = PQexecParams(
        db.get(),
        "UPDATE materials SET title = $2, type = $3, content = $4 "
        "WHERE id = $1 "
        "RETURNING id, lesson_id, title, type, content",
        4,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to update material: " + error);
    }

    Material material;
    material.id = std::stoi(PQgetvalue(res, 0, 0));
    material.lessonId = std::stoi(PQgetvalue(res, 0, 1));
    material.title = PQgetvalue(res, 0, 2);
    material.type = PQgetvalue(res, 0, 3);
    material.content = PQgetvalue(res, 0, 4);
    PQclear(res);
    return material;
}

void PostgresMaterialRepository::deleteMaterial(int materialId) {
    std::lock_guard<std::mutex> lock(db.mutex());

    std::string materialIdValue = std::to_string(materialId);
    const char* params[] = {materialIdValue.c_str()};

    PGresult* res = PQexecParams(
        db.get(),
        "DELETE FROM materials WHERE id = $1",
        1,
        nullptr,
        params,
        nullptr,
        nullptr,
        0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::string error = PQerrorMessage(db.get());
        PQclear(res);
        throw std::runtime_error("Failed to delete material: " + error);
    }

    PQclear(res);
}
