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

Test InMemoryTestRepository::createTest(int courseId, const std::string& title) {
    int nextId = tests.empty() ? 1 : tests.back().id + 1;
    Test test{nextId, courseId, title};
    tests.push_back(test);
    return test;
}
