#pragma once

#include <QWidget>

#include "../models/sessiondata.h"

class QLabel;

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);

    void setSession(const SessionData &session);

private:
    QLabel *m_nameValueLabel;
    QLabel *m_roleValueLabel;
    QLabel *m_tokenValueLabel;
    QLabel *m_userIdValueLabel;
};
