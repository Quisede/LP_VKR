#include "InMemoryAttemptRepository.h"

InMemoryAttemptRepository::InMemoryAttemptRepository() {
    // можно добавить начальные данные, если нужно
    attempts.push_back(Attempt{1, 1, 1, 85, 100, 85.0, true});
    attempts.push_back(Attempt{2, 1, 1, 90, 100, 90.0, true});
    attempts.push_back(Attempt{3, 2, 1, 75, 100, 75.0, true});
}   

void InMemoryAttemptRepository::saveAttempt(const Attempt& attempt) {
    attempts.push_back(attempt);
}

std::vector<Attempt> InMemoryAttemptRepository::getAttemptsForUser(int userId) {

    std::vector<Attempt> result;

    for (const auto& a : attempts) {
        if (a.userId == userId) {
            result.push_back(a);
        }
    }

    return result;
}

CourseAnalytics InMemoryAttemptRepository::getCourseAnalytics(int courseId) {
    CourseAnalytics analytics;

    for (const auto& attempt : attempts) {
        if (attempt.testId != courseId) {
            continue;
        }

        analytics.attemptsCount += 1;
        analytics.averagePercentage += attempt.percentage;
        analytics.rows.push_back({
            "User #" + std::to_string(attempt.userId),
            "Test #" + std::to_string(attempt.testId),
            attempt.score,
            attempt.total,
            attempt.percentage,
            attempt.passed
        });
    }

    if (analytics.attemptsCount > 0) {
        analytics.averagePercentage /= analytics.attemptsCount;
    }

    return analytics;
}
