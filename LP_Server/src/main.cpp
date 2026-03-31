//
//  main.cpp
//  LP_Server
//
//  Created by Даниил Дружинин on 07.01.2026.
//

#include <iostream>
#include <cstdlib>
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

#include "repositories/InMemoryTestRepository.h"
#include "services/TestService.h"
#include "controllers/TestController.h"

#include "repositories/InMemoryQuestionRepository.h"
#include "services/QuestionService.h"
#include "controllers/QuestionController.h"

// #include "repositories/InMemoryAttemptRepository.h"
#include "repositories/PostgresAttemptRepository.h"
#include "services/AttemptService.h"
#include "controllers/AttemptController.h"

int main() {
    try {
    InMemoryUserRepository userRepo; // хранилище пользователей в памяти
    SimplePasswordHasher hasher; // хэшер паролей
    AuthService authService(userRepo, hasher); // сервис аутентификации, принимает зависимости через конструктор

    InMemoryCourseRepository courseRepo; // хранилище курсов в памяти
    InMemoryEnrollmentRepository enrollRepo; // хранилище связей
    CourseService courseService(courseRepo, enrollRepo); // сервис аутентификации, принимает зависимости через конструктор

    InMemoryLessonRepository lessonRepo;

    InMemoryTestRepository testRepo;

    InMemoryQuestionRepository questionRepo;

    // InMemoryAttemptRepository attemptRepo;
    const char* dbHost = std::getenv("PGHOST");
    const char* dbPort = std::getenv("PGPORT");
    const char* dbName = std::getenv("PGDATABASE");
    const char* dbUser = std::getenv("PGUSER");
    const char* dbPassword = std::getenv("PGPASSWORD");
    const char* systemUser = std::getenv("USER");

    std::string conninfo = "host=" + std::string(dbHost ? dbHost : "127.0.0.1");
    conninfo += " port=" + std::string(dbPort ? dbPort : "5432");
    conninfo += " dbname=" + std::string(dbName ? dbName : "lms");
    conninfo += " user=" + std::string(dbUser ? dbUser : (systemUser ? systemUser : ""));

    if(dbPassword && std::string(dbPassword).empty() == false) {
        conninfo += " password=" + std::string(dbPassword);
    }

    PostgresConnection conn(conninfo);

    PostgresAttemptRepository attemptRepo(conn);

    LessonService lessonService(lessonRepo);

    TestService testService(testRepo, enrollRepo);

    QuestionService questionService(questionRepo, attemptRepo);

    AttemptService attemptService(attemptRepo);

    AttemptController attemptController(attemptService);

    LessonController lessonController(lessonService);

    TestController testController(testService);

    QuestionController questionController(questionService);

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

    testController.registerRoutes(server);

    questionController.registerRoutes(server);

    attemptController.registerRoutes(server);  
    
    std::cout << "Server running on the http://localhost:8080\n";
    
    //слушаем вме сетевые интерфейсы, порт 8080
    server.listen("0.0.0.0", 8080);
    } catch(const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
