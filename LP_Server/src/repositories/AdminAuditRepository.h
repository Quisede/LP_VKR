#pragma once

#include <string>
#include <vector>

#include "../models/AdminAuditEvent.h"

class AdminAuditRepository {
public:
    virtual ~AdminAuditRepository() = default;

    virtual void recordEvent(
        int adminId,
        const std::string& action,
        const std::string& targetType,
        int targetId,
        const std::string& details) = 0;

    virtual std::vector<AdminAuditEvent> getRecentEvents(int limit) = 0;
};
