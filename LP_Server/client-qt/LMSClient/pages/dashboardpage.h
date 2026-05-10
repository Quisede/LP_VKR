#pragma once

#include <QWidget>
#include <QVector>

#include "../models/adminoverviewmodel.h"
#include "../models/attemptmodel.h"
#include "../models/coursemodel.h"
#include "../models/sessiondata.h"

class QLabel;
class QListWidget;
class QFrame;
class QPushButton;

class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);

    void setRoleMode(const QString &role);
    void setSession(const SessionData &session);
    void setCourses(const QVector<CourseData> &courses);
    void setAttempts(const QVector<AttemptData> &attempts);
    void setAdminOverview(const AdminOverviewData &overview);

signals:
    void openCoursesRequested();
    void openCreateCourseRequested();
    void openStudentsRequested();
    void openAnalyticsRequested();
    void openResultsRequested();
    void openTestsRequested();
    void openUsersRequested();

private:
    QString m_role = "Student";
    QVector<CourseData> m_courses;
    QVector<AttemptData> m_attempts;
    AdminOverviewData m_adminOverview;
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
    QLabel *m_actionsTitleLabel;
    QListWidget *m_recentCoursesList;
    QListWidget *m_focusList;
    QPushButton *m_primaryActionButton;
    QPushButton *m_secondaryActionButton;
    QPushButton *m_tertiaryActionButton;
    QPushButton *m_quaternaryActionButton;
};
