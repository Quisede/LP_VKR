#pragma once

#include <vector>
#include "../models/Attempt.h"
#include "../models/CourseAnalytics.h"
#include "../models/StudentCourseAttempt.h"

class AttemptRepository {
public:
    virtual ~AttemptRepository() = default;

    virtual void saveAttempt(const Attempt& attempt) = 0;
    virtual std::vector<Attempt> getAttemptsForUser(int userId) = 0;
    virtual int countAttemptsForUserTest(int userId, int testId) = 0;
    virtual CourseAnalytics getCourseAnalytics(int courseId) = 0;
    virtual std::vector<StudentCourseAttempt> getStudentCourseAttempts(int courseId, int studentId) = 0;
};
