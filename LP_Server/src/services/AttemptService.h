#pragma once

#include <vector>
#include "../models/Attempt.h"
#include "../models/CourseAnalytics.h"
#include "../models/User.h"
#include "../services/CourseService.h"
#include "../repositories/AttemptRepository.h"

class AttemptService {
public:
    AttemptService(AttemptRepository& repo, CourseService& courseService);

    std::vector<Attempt> getAttemptsForUser(int currentUserId, UserRole role, int requestedUserId);
    CourseAnalytics getCourseAnalytics(int currentUserId, UserRole role, int courseId);

private:
    AttemptRepository& attemptRepository;
    CourseService& courseService;
};
