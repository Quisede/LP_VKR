//
//  main.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 07.01.2026.
//

#include <iostream>
#include <libpq-fe.h>
#include "httplib.h"
#include "json.hpp"

#include "services/AuthService.h"
#include "services/SimplePasswordHasher.h"
#include "repositories/InMemoryUserRepository.h"
#include "controllers/AuthController.h"

#include "services/CourseService.h"
#include "repositories/InMemoryCourseRepository.h"
#include "controllers/CourseController.h"

#include "repositories/InMemoryEnrollmentRepository.h"
#include "repositories/InMemoryLessonRepository.h"
#include "services/LessonService.h"
#include "controllers/LessonController.h"

#include "repositories/InMemoryMaterialRepository.h"
#include "services/MaterialService.h"
#include "controllers/MaterialController.h"

int main() {
    InMemoryUserRepository userRepo; // хранилище пользователей в памяти
    SimplePasswordHasher hasher; // хэшер паролей
    AuthService authService(userRepo, hasher); // сервис аутентификации, принимает зависимости через конструктор

    InMemoryCourseRepository courseRepo; // хранилище курсов в памяти
    InMemoryEnrollmentRepository enrollRepo; // хранилище связей
    CourseService courseService(courseRepo, enrollRepo); // сервис аутентификации, принимает зависимости через конструктор

    InMemoryLessonRepository lessonRepo;

    LessonService lessonService(lessonRepo);

    LessonController lessonController(lessonService);

    InMemoryMaterialRepository materialRepo;

    MaterialService materialService(materialRepo);

    MaterialController materialController(materialService);
    
    // тестовый юзер
    authService.registerUser("student1", "12345", UserRole::Student);
    
    // создание http сервера
    httplib::Server server;
    
    // создается контроллер аутентификации
    AuthController authController(authService);
    
    // создается контроллер для курсов
    CourseController courseController(courseService);
    
    // регистрируем новый обработчик аутентификации в сервере
    authController.registerRoutes(server);
    
    // регистрируем новый обработчик курсов в сервере
    courseController.registerRoutes(server);

    lessonController.registerRoutes(server);

    materialController.registerRoutes(server);
    
    std::cout << "Server running on the http://localhost:8080\n";
    
    //слушаем вме сетевые интерфейсы, порт 8080
    server.listen("0.0.0.0", 8080);
}
