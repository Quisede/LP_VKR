#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPair>
#include <QPushButton>
#include <QString>
#include <QVector>

#include "models/adminoverviewmodel.h"
#include "models/attemptmodel.h"
#include "models/coursemodel.h"
#include "models/lessonmodel.h"
#include "models/materialmodel.h"
#include "models/questionmodel.h"
#include "models/sessiondata.h"
#include "models/teacherstudentattemptmodel.h"
#include "models/testmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ApiClient;
class AdminAuditPage;
class AdminGroupsPage;
class AdminUsersPage;
class AttemptsPage;
class CourseDetailsPage;
class CoursesPage;
class DashboardPage;
class DeadlinesPage;
class ProfilePage;
class TeacherCourseBuilderPage;
class TeacherCreateCoursePage;
class TeacherAnalyticsPage;
class TeacherStudentsPage;
class TeacherTestEditorPage;
class TestRunnerPage;
class QLabel;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ApiClient *apiClient, QWidget *parent = nullptr);
    ~MainWindow() override;

    void setSession(const SessionData &session);
    void automationOpenPage(const QString &pageKey, const QString &courseTitle = QString());

signals:
    void logoutRequested();

private slots:
    void showHomePage();
    void showCoursesPage();
    void showCreateCoursePage();
    void showTestPage();
    void showResultsPage();
    void showStudentsPage();
    void showProfilePage();
    void onLogoutClicked();

    void onCourseOpened(const CourseData &course);
    void onCourseBuilderRequested(const CourseData &course);
    void onEnrollRequested(int courseId);
    void onBackToCoursesRequested();
    void onTestSelected(int testId, const QString &title);
    void onBackFromTestRequested();
    void onSubmitTestRequested(int testId, const QVector<QPair<int, int>> &answers);

private:
    bool isAdminMode() const;
    bool isTeacherMode() const;
    void applyRoleMode();
    void setActiveSection(QPushButton *button);
    void setHeader(const QString &title, const QString &subtitle);
    void showStatus(const QString &status);
    void updateFooterStatus(const QString &status, bool connected);
    void showCourseDetailsPage();
    void showTeacherCourseBuilderPage();
    void showTeacherTestEditorPage();
    void showTestRunnerPage();
    void checkBackendHealth();

    void loadCourses();
    void loadAttempts();
    void loadAdminOverview();
    void loadAdminUsers();
    void loadAdminGroups();
    void loadAdminAudit();
    void loadCourseContent(int courseId);
    void loadCourseLessonsAndMaterials(int courseId);
    void loadCourseTests(int courseId);
    void loadManagedQuestions(int testId);
    void loadTeacherCourseStudents(int courseId);
    void loadTeacherStudentAttempts(int courseId, int studentId, const QString &studentLogin);
    void loadTeacherAnalytics(int courseId);
    void refreshSelectedCourseFromCache();
    void applyStudentProgressToCourses();
    QVector<CourseData> visibleCoursesForCurrentRole(const QVector<CourseData> &courses) const;
    CourseData automationCourseCandidate(const QString &courseTitle) const;

    Ui::MainWindow *ui;
    ApiClient *m_apiClient;

    AdminUsersPage *m_adminUsersPage;
    AdminGroupsPage *m_adminGroupsPage;
    AdminAuditPage *m_adminAuditPage;
    DashboardPage *m_dashboardPage;
    DeadlinesPage *m_deadlinesPage;
    CoursesPage *m_coursesPage;
    TeacherCreateCoursePage *m_teacherCreateCoursePage;
    CourseDetailsPage *m_courseDetailsPage;
    TeacherCourseBuilderPage *m_teacherCourseBuilderPage;
    TeacherTestEditorPage *m_teacherTestEditorPage;
    TestRunnerPage *m_testRunnerPage;
    AttemptsPage *m_attemptsPage;
    TeacherStudentsPage *m_teacherStudentsPage;
    TeacherAnalyticsPage *m_teacherAnalyticsPage;
    ProfilePage *m_profilePage;

    QPushButton *m_createCourseButton = nullptr;
    QPushButton *m_studentsButton = nullptr;
    QLabel *m_toastLabel = nullptr;
    QTimer *m_connectionTimer = nullptr;

    SessionData m_session;
    QVector<CourseData> m_courses;
    QVector<AttemptData> m_attempts;
    AdminOverviewData m_adminOverview;
    QVector<LessonData> m_selectedLessons;
    QVector<MaterialData> m_selectedMaterials;
    QVector<TestData> m_selectedTests;
    QVector<QuestionData> m_selectedQuestions;
    CourseData m_selectedCourse;
    TestData m_selectedTest;
};

#endif // MAINWINDOW_H
