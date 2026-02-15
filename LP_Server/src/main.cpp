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

int main() {
    InMemoryUserRepository userRepo; // хранилище пользователей в памяти
    SimplePasswordHasher hasher; // хэшер паролей
    AuthService authService(userRepo, hasher); // сервис аутентификации, принимает зависимости через конструктор
    InMemoryCourseRepository courseRepo;; // хранилище курсов в памяти
    CourseService courseService(courseRepo); // сервис аутентификации, принимает зависимости через конструктор
    
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
    
    std::cout << "Server running on the http://localhost:8080\n";
    
    //слушаем вме сетевые интерфейсы, порт 8080
    server.listen("0.0.0.0", 8080);
}
