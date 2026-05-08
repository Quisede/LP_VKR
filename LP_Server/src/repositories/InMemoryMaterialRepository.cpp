#include "InMemoryMaterialRepository.h"
#include <algorithm>
#include <stdexcept>

InMemoryMaterialRepository::InMemoryMaterialRepository() {
    materials.push_back({
        1,
        1,
        "Intro text",
        "text",
        "Welcome to first test lesson"
    });

    materials.push_back({
        2,
        1,
        "Variables",
        "text",
        "Welcome to second test lesson"
    });
    
    materials.push_back({
        3,
        2,
        "OOP",
        "video",
        "https://rutube.ru/video/fa50d763b9ae0c25b1841bf78fe07bf0/?r=wd"
    });

}

std::vector<Material> InMemoryMaterialRepository::getMaterialsForLesson(int lessonId) {
    std::vector<Material> result;

    for(const auto& material : materials) {
        if(material.lessonId == lessonId) {
            result.push_back(material);
        }
    }

    return result;
}

std::optional<Material> InMemoryMaterialRepository::getMaterialById(int materialId) {
    for (const auto &material : materials) {
        if (material.id == materialId) {
            return material;
        }
    }

    return std::nullopt;
}

Material InMemoryMaterialRepository::createMaterial(
    int lessonId,
    const std::string& title,
    const std::string& type,
    const std::string& content) {
    int nextId = materials.empty() ? 1 : materials.back().id + 1;
    Material material{nextId, lessonId, title, type, content};
    materials.push_back(material);
    return material;
}

Material InMemoryMaterialRepository::updateMaterial(
    int materialId,
    const std::string& title,
    const std::string& type,
    const std::string& content) {
    for (auto &material : materials) {
        if (material.id == materialId) {
            material.title = title;
            material.type = type;
            material.content = content;
            return material;
        }
    }

    throw std::runtime_error("Material not found");
}

void InMemoryMaterialRepository::deleteMaterial(int materialId) {
    materials.erase(
        std::remove_if(
            materials.begin(),
            materials.end(),
            [materialId](const Material &material) { return material.id == materialId; }),
        materials.end());
}
