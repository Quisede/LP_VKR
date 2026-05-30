#pragma once

#include <QString>

struct AdminUserData {
    int id = -1;
    QString login;
    QString role;
    QString firstName;
    QString lastName;
    QString groupName;
    QString email;
    QString phone;
    bool editable = true;
};
