#pragma once

#include "AttemptRepository.h"
#include <vector>

class InMemoryAttemptRepository : public AttemptRepository {
    public:
        InMemoryAttemptRepository();

        void saveAttempt(const Attempt& attempt) override;
        std::vector<Attempt> getAttemptsForUser(int userId) override;
        CourseAnalytics getCourseAnalytics(int courseId) override;
        std::vector<StudentCourseAttempt> getStudentCourseAttempts(int courseId, int studentId) override;
    private:
        std::vector<Attempt> attempts;
};
