//
//  CourseService.h
//  LP_Server
//
//  Created by Даниил Дружинин on 10.01.2026.
//

#pragma once

#include <vector>
#include "../models/Course.h"
#include "../models/User.h"
#include "../repositories/CourseRepository.h"

class CourseService {
public:
    CourseService(CourseRepository& repo);
    
    std::vector<Course> getCoursesForUser(int userId, UserRole role);
    
private:
    CourseRepository& courseRepository;
};
