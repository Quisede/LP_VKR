#pragma once

#include <QWidget>
#include <QVector>

#include "../models/attemptmodel.h"
#include "../models/coursemodel.h"
#include "../models/sessiondata.h"

class QLabel;

class ProfilePage : public QWidget
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);

    void setSession(const SessionData &session);
    void setCourses(const QVector<CourseData> &courses);
    void setAttempts(const QVector<AttemptData> &attempts);

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
    QLabel *m_tokenValueLabel;
    QLabel *m_userIdValueLabel;
};
