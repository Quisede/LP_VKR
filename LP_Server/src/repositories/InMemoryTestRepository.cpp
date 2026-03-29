#include "InMemoryTestRepository.h"

InMemoryTestRepository::InMemoryTestRepository() {
    // заполняем тестовыми данными
    tests.push_back({1, 1, "C++ Basic Test"});
    tests.push_back({2, 1, "Variables Test"});
    tests.push_back({3, 2, "OOP Test"});
}

std::vector<Test> InMemoryTestRepository::getTestsForCourse(int courseId) {
    std::vector<Test> result;

    for(const auto& test : tests) {
        if(test.courseId == courseId) {
            result.push_back(test);
        }
    }

    return result;
}