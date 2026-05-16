#pragma once

#include "AdminAuditRepository.h"
#include "../database/PostgresConnection.h"

class PostgresAdminAuditRepository : public AdminAuditRepository {
public:
    explicit PostgresAdminAuditRepository(PostgresConnection& connection);

    void recordEvent(
        int adminId,
        const std::string& action,
        const std::string& targetType,
        int targetId,
        const std::string& details) override;

    std::vector<AdminAuditEvent> getRecentEvents(int limit) override;

private:
    void ensureTable();

    PostgresConnection& db;
};
