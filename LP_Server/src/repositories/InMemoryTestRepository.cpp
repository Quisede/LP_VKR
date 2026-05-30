#include "InMemoryTestRepository.h"
#include <algorithm>
#include <stdexcept>

InMemoryTestRepository::InMemoryTestRepository() {
    // заполняем тестовыми данными
    tests.push_back({1, 1, "C++ Basic Test", "active", "", true, 2, 30});
    tests.push_back({2, 1, "Variables Test", "active", "", true, 3, 20});
    tests.push_back({3, 2, "OOP Test", "active", "", true, 2, 25});
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

std::vector<Test> InMemoryTestRepository::getTestsForCourseForStudent(int courseId, int) {
    return getTestsForCourse(courseId);
}

std::optional<Test> InMemoryTestRepository::getTestById(int testId) {
    for (const auto& test : tests) {
        if (test.id == testId) {
            return test;
        }
    }

    return std::nullopt;
}

Test InMemoryTestRepository::createTest(int courseId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) {
    int nextId = tests.empty() ? 1 : tests.back().id + 1;
    Test test{nextId, courseId, title, status, deadlineAt, status == "active", maxAttempts, timeLimitMinutes};
    tests.push_back(test);
    return test;
}

Test InMemoryTestRepository::updateTest(int testId, const std::string& title, const std::string& status, const std::string& deadlineAt, int maxAttempts, int timeLimitMinutes) {
    for (auto& test : tests) {
        if (test.id == testId) {
            test.title = title;
            test.status = status;
            test.deadlineAt = deadlineAt;
            test.available = status == "active";
            test.maxAttempts = maxAttempts;
            test.timeLimitMinutes = timeLimitMinutes;
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
