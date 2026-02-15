//
//  Course.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include <string>

struct Course {
    int id;
    std::string title;
    std::string description;
    int teacherId;
};
