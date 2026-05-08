#pragma once

#include "AttemptRepository.h"
#include <vector>

class InMemoryAttemptRepository : public AttemptRepository {
    public:
        InMemoryAttemptRepository();

        void saveAttempt(const Attempt& attempt) override;
        std::vector<Attempt> getAttemptsForUser(int userId) override;
        CourseAnalytics getCourseAnalytics(int courseId) override;
    private:
        std::vector<Attempt> attempts;
};
