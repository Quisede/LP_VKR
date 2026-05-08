#pragma once

#include <QWidget>
#include <QVector>

#include "../models/attemptmodel.h"
#include "../models/coursemodel.h"
#include "../models/sessiondata.h"

class QLabel;
class QListWidget;
class QFrame;

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);

    void setRoleMode(const QString &role);
    void setSession(const SessionData &session);
    void setCourses(const QVector<CourseData> &courses);
    void setAttempts(const QVector<AttemptData> &attempts);

private:
    QString m_role = "Student";
    QVector<CourseData> m_courses;
    QVector<AttemptData> m_attempts;
    void refreshSummary();
    void refreshRecentCourses();
    void refreshFocus();
    QLabel *m_welcomeLabel;
    QLabel *m_hintLabel;
    QLabel *m_recentCoursesTitleLabel;
    QLabel *m_focusTitleLabel;
    QLabel *m_coursesTitleLabel;
    QLabel *m_testsTitleLabel;
    QLabel *m_attemptsTitleLabel;
    QLabel *m_coursesValueLabel;
    QLabel *m_testsValueLabel;
    QLabel *m_attemptsValueLabel;
    QLabel *m_coursesCaptionLabel;
    QLabel *m_testsCaptionLabel;
    QLabel *m_attemptsCaptionLabel;
    QListWidget *m_recentCoursesList;
    QListWidget *m_focusList;
};
