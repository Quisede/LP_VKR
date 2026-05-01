#include "MaterialService.h"
#include <stdexcept>

MaterialService::MaterialService(MaterialRepository& repo)
    :materialRepository(repo) {}

std::vector<Material> MaterialService::getMaterialsForLesson(int lessonId) {
    return materialRepository.getMaterialsForLesson(lessonId);
}

Material MaterialService::createMaterial(
    int lessonId,
    const std::string& title,
    const std::string& type,
    const std::string& content) {
    if (title.empty()) {
        throw std::invalid_argument("Material title must not be empty");
    }

    if (content.empty()) {
        throw std::invalid_argument("Material content must not be empty");
    }

    if (type != "text" && type != "video" && type != "link") {
        throw std::invalid_argument("Material type must be one of: text, video, link");
    }

    return materialRepository.createMaterial(lessonId, title, type, content);
}
