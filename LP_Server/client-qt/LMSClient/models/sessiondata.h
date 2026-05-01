#pragma once

#include <QString>
#include <QMetaType>

struct SessionData {
    QString token;
    int userId = -1;
    QString role;
    QString login;
};

Q_DECLARE_METATYPE(SessionData)
