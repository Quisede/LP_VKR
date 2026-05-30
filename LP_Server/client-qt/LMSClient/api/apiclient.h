#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QStringList>
#include <QVector>
#include <functional>

#include "../models/attemptmodel.h"
#include "../models/adminauditmodel.h"
#include "../models/adminusermodel.h"
#include "../models/adminoverviewmodel.h"
#include "../models/coursemodel.h"
#include "../models/coursestudentmodel.h"
#include "../models/lessonmodel.h"
#include "../models/materialmodel.h"
#include "../models/questionmodel.h"
#include "../models/sessiondata.h"
#include "../models/teacheranalyticsmodel.h"
#include "../models/teacherstudentattemptmodel.h"
#include "../models/testmodel.h"

class ApiClient : public QObject
{
    Q_OBJECT

public:
    explicit ApiClient(QObject *parent = nullptr);

    void setBaseUrl(const QString &baseUrl);
    void setToken(const QString &token);
    QString token() const;

    void checkHealth(
        QObject *context,
        std::function<void(const QString &status)> onSuccess,
        std::function<void(const QString &error)> onError);

    void login(
        const QString &login,
        const QString &password,
        QObject *context,
        std::function<void(const SessionData &session)> onSuccess,
        std::function<void(const QString &error)> onError);

    void registerStudent(
        const QString &login,
        const QString &password,
        const QString &firstName,
        const QString &lastName,
        const QString &groupName,
        const QString &email,
        const QString &phone,
        QObject *context,
        std::function<void(const SessionData &session)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getCurrentProfile(
        QObject *context,
        std::function<void(const SessionData &session)> onSuccess,
        std::function<void(const QString &error)> onError);

    void changePassword(
        const QString &oldPassword,
        const QString &newPassword,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void getCourses(
        QObject *context,
        std::function<void(const QVector<CourseData> &courses)> onSuccess,
        std::function<void(const QString &error)> onError);

    void createCourse(
        const QString &title,
        const QString &description,
        QObject *context,
        std::function<void(const CourseData &course)> onSuccess,
        std::function<void(const QString &error)> onError);

    void updateCourse(
        int courseId,
        const QString &title,
        const QString &description,
        QObject *context,
        std::function<void(const CourseData &course)> onSuccess,
        std::function<void(const QString &error)> onError);

    void deleteCourse(
        int courseId,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void enrollCourse(
        int courseId,
        QObject *context,
        std::function<void(const QString &message)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getLessons(
        int courseId,
        QObject *context,
        std::function<void(const QVector<LessonData> &lessons)> onSuccess,
        std::function<void(const QString &error)> onError);

    void createLesson(
        int courseId,
        const QString &title,
        const QString &content,
        QObject *context,
        std::function<void(const LessonData &lesson)> onSuccess,
        std::function<void(const QString &error)> onError);

    void updateLesson(
        int lessonId,
        const QString &title,
        const QString &content,
        QObject *context,
        std::function<void(const LessonData &lesson)> onSuccess,
        std::function<void(const QString &error)> onError);

    void deleteLesson(
        int lessonId,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void markLessonCompleted(
        int lessonId,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void getMaterials(
        int lessonId,
        QObject *context,
        std::function<void(const QVector<MaterialData> &materials)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getCourseMaterials(
        int courseId,
        QObject *context,
        std::function<void(const QVector<MaterialData> &materials)> onSuccess,
        std::function<void(const QString &error)> onError);

    void createMaterial(
        int lessonId,
        const QString &title,
        const QString &type,
        const QString &content,
        QObject *context,
        std::function<void(const MaterialData &material)> onSuccess,
        std::function<void(const QString &error)> onError);

    void updateMaterial(
        int materialId,
        const QString &title,
        const QString &type,
        const QString &content,
        QObject *context,
        std::function<void(const MaterialData &material)> onSuccess,
        std::function<void(const QString &error)> onError);

    void deleteMaterial(
        int materialId,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void downloadMaterialFile(
        int materialId,
        QObject *context,
        std::function<void(const MaterialFileData &file)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getTests(
        int courseId,
        QObject *context,
        std::function<void(const QVector<TestData> &tests)> onSuccess,
        std::function<void(const QString &error)> onError);

    void createTest(
        int courseId,
        const QString &title,
        const QString &status,
        const QString &deadlineAt,
        int maxAttempts,
        int timeLimitMinutes,
        QObject *context,
        std::function<void(const TestData &test)> onSuccess,
        std::function<void(const QString &error)> onError);

    void updateTest(
        int testId,
        const QString &title,
        const QString &status,
        const QString &deadlineAt,
        int maxAttempts,
        int timeLimitMinutes,
        QObject *context,
        std::function<void(const TestData &test)> onSuccess,
        std::function<void(const QString &error)> onError);

    void deleteTest(
        int testId,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void getQuestions(
        int testId,
        QObject *context,
        std::function<void(const QVector<QuestionData> &questions)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getManageQuestions(
        int testId,
        QObject *context,
        std::function<void(const QVector<QuestionData> &questions)> onSuccess,
        std::function<void(const QString &error)> onError);

    void createQuestion(
        int testId,
        const QString &text,
        const QStringList &options,
        int correctOptionIndex,
        QObject *context,
        std::function<void(const QuestionData &question)> onSuccess,
        std::function<void(const QString &error)> onError);

    void updateQuestion(
        int questionId,
        const QString &text,
        const QStringList &options,
        int correctOptionIndex,
        QObject *context,
        std::function<void(const QuestionData &question)> onSuccess,
        std::function<void(const QString &error)> onError);

    void deleteQuestion(
        int questionId,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void submitTest(
        int testId,
        const QVector<QPair<int, int>> &answers,
        QObject *context,
        std::function<void(const AttemptData &result)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getAttempts(
        int userId,
        QObject *context,
        std::function<void(const QVector<AttemptData> &attempts)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getAdminUsers(
        QObject *context,
        std::function<void(const QVector<AdminUserData> &users)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getAdminGroups(
        QObject *context,
        std::function<void(const QStringList &groups)> onSuccess,
        std::function<void(const QString &error)> onError);

    void createAdminGroup(
        const QString &groupName,
        QObject *context,
        std::function<void(const QString &groupName)> onSuccess,
        std::function<void(const QString &error)> onError);

    void renameAdminGroup(
        const QString &oldName,
        const QString &newName,
        QObject *context,
        std::function<void(const QString &groupName)> onSuccess,
        std::function<void(const QString &error)> onError);

    void deleteAdminGroup(
        const QString &groupName,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void getAdminOverview(
        QObject *context,
        std::function<void(const AdminOverviewData &overview)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getAdminAudit(
        QObject *context,
        std::function<void(const QVector<AdminAuditEventData> &events)> onSuccess,
        std::function<void(const QString &error)> onError);

    void createAdminUser(
        const QString &login,
        const QString &password,
        const QString &role,
        const QString &firstName,
        const QString &lastName,
        const QString &groupName,
        const QString &email,
        const QString &phone,
        QObject *context,
        std::function<void(const AdminUserData &user)> onSuccess,
        std::function<void(const QString &error)> onError);

    void updateAdminUserRole(
        int userId,
        const QString &role,
        QObject *context,
        std::function<void(const AdminUserData &user)> onSuccess,
        std::function<void(const QString &error)> onError);

    void deleteAdminUser(
        int userId,
        QObject *context,
        std::function<void()> onSuccess,
        std::function<void(const QString &error)> onError);

    void getCourseStudents(
        int courseId,
        QObject *context,
        std::function<void(const QVector<CourseStudentData> &students)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getTeacherStudentAttempts(
        int courseId,
        int studentId,
        QObject *context,
        std::function<void(const QVector<TeacherStudentAttemptData> &attempts)> onSuccess,
        std::function<void(const QString &error)> onError);

    void getCourseAnalytics(
        int courseId,
        QObject *context,
        std::function<void(const TeacherCourseAnalyticsData &analytics)> onSuccess,
        std::function<void(const QString &error)> onError);

private:
    QNetworkRequest createRequest(const QString &path, bool withAuth = true) const;
    QString extractErrorMessage(const QByteArray &responseData, const QString &fallback) const;

    QString m_baseUrl = "http://localhost:8080";
    QString m_token;
    QNetworkAccessManager m_networkManager;
};
