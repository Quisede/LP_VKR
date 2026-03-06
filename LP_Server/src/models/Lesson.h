#pragma once

#include <string>

struct Lesson {
    int id; // идентификатор урока
    int courseId; // к какому курсу относится
    std::string title; // название урока
    std::string content; // текст урока
};