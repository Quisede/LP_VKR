#pragma once

#include <QString>

struct AdminUserData {
    int id = -1;
    QString login;
    QString role;
    bool editable = true;
};
