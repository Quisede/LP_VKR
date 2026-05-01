#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPair>
#include <QPushButton>
#include <QString>
#include <QVector>

#include "models/attemptmodel.h"
#include "models/coursemodel.h"
#include "models/lessonmodel.h"
#include "models/materialmodel.h"
#include "models/sessiondata.h"
#include "models/testmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ApiClient;
class AttemptsPage;
class CourseDetailsPage;
class CoursesPage;
class DashboardPage;
class ProfilePage;
class TeacherCourseBuilderPage;
class TeacherCreateCoursePage;
class TeacherStudentsPage;
class QWidget;
class TestRunnerPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(ApiClient *apiClient, QWidget *parent = nullptr);
    ~MainWindow() override;

    void setSession(const SessionData &session);

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
    bool isTeacherMode() const;
    void applyRoleMode();
    void setActiveSection(QPushButton *button);
    void setHeader(const QString &title, const QString &subtitle);
    void showStatus(const QString &status);
    void showCourseDetailsPage();
    void showTeacherCourseBuilderPage();
    void showTestRunnerPage();
    QWidget *createTeacherAnalyticsPage();

    void loadCourses();
    void loadAttempts();
    void loadCourseContent(int courseId);
    void loadCourseLessonsAndMaterials(int courseId);
    void loadCourseTests(int courseId);
    void loadTeacherCourseStudents(int courseId);
    void refreshSelectedCourseFromCache();
    QVector<CourseData> visibleCoursesForCurrentRole(const QVector<CourseData> &courses) const;

    Ui::MainWindow *ui;
    ApiClient *m_apiClient;

    DashboardPage *m_dashboardPage;
    CoursesPage *m_coursesPage;
    TeacherCreateCoursePage *m_teacherCreateCoursePage;
    CourseDetailsPage *m_courseDetailsPage;
    TeacherCourseBuilderPage *m_teacherCourseBuilderPage;
    TestRunnerPage *m_testRunnerPage;
    AttemptsPage *m_attemptsPage;
    TeacherStudentsPage *m_teacherStudentsPage;
    QWidget *m_teacherAnalyticsPage;
    ProfilePage *m_profilePage;

    QPushButton *m_createCourseButton = nullptr;
    QPushButton *m_studentsButton = nullptr;

    SessionData m_session;
    QVector<CourseData> m_courses;
    QVector<AttemptData> m_attempts;
    QVector<LessonData> m_selectedLessons;
    QVector<MaterialData> m_selectedMaterials;
    QVector<TestData> m_selectedTests;
    CourseData m_selectedCourse;
    TestData m_selectedTest;
};

#endif // MAINWINDOW_H
