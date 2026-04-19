#include "PostgresMaterialRepository.h"
#include <libpq-fe.h>

PostgresMaterialRepository::PostgresMaterialRepository(PostgresConnection& connection)
    : db(connection) {}

std::vector<Material> PostgresMaterialRepository::getMaterialsForLesson(int lessonId) {
    std::vector<Material> materials;

    std::string query = "SELECT id, lesson_id, title, type, content FROM materials WHERE lesson_id = " + std::to_string(lessonId) + ";";

    PGresult* res = PQexec(db.get(), query.c_str());

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