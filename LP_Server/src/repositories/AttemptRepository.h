#pragma once

#include <vector>
#include "../models/Attempt.h"
#include "../models/CourseAnalytics.h"

class AttemptRepository {
public:
    virtual ~AttemptRepository() = default;

    virtual void saveAttempt(const Attempt& attempt) = 0;
    virtual std::vector<Attempt> getAttemptsForUser(int userId) = 0;
    virtual CourseAnalytics getCourseAnalytics(int courseId) = 0;
};
