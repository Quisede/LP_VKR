#include "InMemoryMaterialRepository.h"

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