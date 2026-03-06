#include "MaterialService.h"

MaterialService::MaterialService(MaterialRepository& repo)
    :materialRepository(repo) {}

std::vector<Material> MaterialService::getMaterialsForLesson(int lessonId) {
    return materialRepository.getMaterialsForLesson(lessonId);
}