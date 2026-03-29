#pragma once

#include <vector>

// модель ответа
struct SubmittedAnswer {
    int questionId;
    int answerId;
};

struct TestSubmission {
    std::vector<SubmittedAnswer> answers;
};