#pragma once

#include <QString>
#include <QMetaType>

struct SessionData {
    QString token;
    int userId = -1;
    QString role;
    QString login;
    QString firstName;
    QString lastName;
    QString fullName;
    QString groupName;
    QString email;
    QString phone;
};

Q_DECLARE_METATYPE(SessionData)
