#include "MaterialService.h"
#include <stdexcept>

namespace {

bool isSupportedMaterialType(const std::string& type) {
    return type == "text"
        || type == "video"
        || type == "link"
        || type == "pdf"
        || type == "doc"
        || type == "docx"
        || type == "file";
}

}

MaterialService::MaterialService(MaterialRepository& repo)
    :materialRepository(repo) {}

std::vector<Material> MaterialService::getMaterialsForLesson(int lessonId) {
    return materialRepository.getMaterialsForLesson(lessonId);
}

std::optional<Material> MaterialService::getMaterialById(int materialId) {
    return materialRepository.getMaterialById(materialId);
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

    if (!isSupportedMaterialType(type)) {
        throw std::invalid_argument("Material type must be one of: text, video, link, pdf, doc, docx, file");
    }

    return materialRepository.createMaterial(lessonId, title, type, content);
}

Material MaterialService::updateMaterial(
    int materialId,
    const std::string& title,
    const std::string& type,
    const std::string& content) {
    if (title.empty()) {
        throw std::invalid_argument("Material title must not be empty");
    }

    if (content.empty()) {
        throw std::invalid_argument("Material content must not be empty");
    }

    if (!isSupportedMaterialType(type)) {
        throw std::invalid_argument("Material type must be one of: text, video, link, pdf, doc, docx, file");
    }

    return materialRepository.updateMaterial(materialId, title, type, content);
}

void MaterialService::deleteMaterial(int materialId) {
    materialRepository.deleteMaterial(materialId);
}
