#pragma once

#include <QWidget>
#include <QVector>

#include "../models/attemptmodel.h"
#include "../models/coursemodel.h"
#include "../models/sessiondata.h"

class QLabel;
class QLineEdit;
class QPushButton;

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);

    void setSession(const SessionData &session);
    void setCourses(const QVector<CourseData> &courses);
    void setAttempts(const QVector<AttemptData> &attempts);
    void showPasswordMessage(const QString &message, bool isError);
    void setPasswordBusy(bool busy);

signals:
    void changePasswordRequested(const QString &oldPassword, const QString &newPassword);

private:
    void refreshLearningSummary();

    SessionData m_session;
    QVector<CourseData> m_courses;
    QVector<AttemptData> m_attempts;
    QLabel *m_introTitleLabel;
    QLabel *m_introTextLabel;
    QLabel *m_summaryOneValueLabel;
    QLabel *m_summaryTwoValueLabel;
    QLabel *m_summaryOneTitleLabel;
    QLabel *m_summaryTwoTitleLabel;
    QLabel *m_nameValueLabel;
    QLabel *m_roleValueLabel;
    QLabel *m_loginValueLabel;
    QLabel *m_groupValueLabel;
    QLabel *m_emailValueLabel;
    QLabel *m_phoneValueLabel;
    QLabel *m_passwordStatusLabel;
    QLineEdit *m_oldPasswordEdit;
    QLineEdit *m_newPasswordEdit;
    QLineEdit *m_repeatPasswordEdit;
    QPushButton *m_changePasswordButton;
};
