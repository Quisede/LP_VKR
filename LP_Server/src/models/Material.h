#pragma once

#include <string>

struct Material {
    int id; // id материала
    int lessonId; // id урока
    std::string title; // название
    std::string type; // тип текст/видео/ссылка
    std::string content; // текст или ссылка
};