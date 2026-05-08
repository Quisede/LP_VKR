#include "InMemoryTestRepository.h"
#include <algorithm>
#include <stdexcept>

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

std::optional<Test> InMemoryTestRepository::getTestById(int testId) {
    for (const auto& test : tests) {
        if (test.id == testId) {
            return test;
        }
    }

    return std::nullopt;
}

Test InMemoryTestRepository::createTest(int courseId, const std::string& title) {
    int nextId = tests.empty() ? 1 : tests.back().id + 1;
    Test test{nextId, courseId, title};
    tests.push_back(test);
    return test;
}

Test InMemoryTestRepository::updateTest(int testId, const std::string& title) {
    for (auto& test : tests) {
        if (test.id == testId) {
            test.title = title;
            return test;
        }
    }

    throw std::runtime_error("Test not found");
}

void InMemoryTestRepository::deleteTest(int testId) {
    tests.erase(
        std::remove_if(
            tests.begin(),
            tests.end(),
            [testId](const Test& test) { return test.id == testId; }),
        tests.end());
}
