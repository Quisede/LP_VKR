#pragma once

#include <QString>

struct AdminAuditEventData {
    int id = -1;
    int adminId = -1;
    QString adminLogin;
    QString action;
    QString targetType;
    int targetId = -1;
    QString details;
    QString createdAt;
};
