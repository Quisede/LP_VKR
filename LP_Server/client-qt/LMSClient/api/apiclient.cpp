#include "apiclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QUrl>

namespace {

QString fileNameFromContentDisposition(const QByteArray &header, int materialId)
{
    const QString value = QString::fromUtf8(header);
    const QRegularExpression quotedExpression("filename=\"([^\"]+)\"");
    const auto quotedMatch = quotedExpression.match(value);
    if (quotedMatch.hasMatch()) {
        return quotedMatch.captured(1);
    }

    const QRegularExpression plainExpression("filename=([^;]+)");
    const auto plainMatch = plainExpression.match(value);
    if (plainMatch.hasMatch()) {
        return plainMatch.captured(1).trimmed();
    }

    return QString("material-%1").arg(materialId);
}

SessionData sessionFromJson(const QJsonObject &obj, const QString &fallbackLogin = {})
{
    SessionData session;
    session.token = obj.value("token").toString();
    session.userId = obj.value("userId").toInt(-1);
    session.role = obj.value("role").toString();
    session.login = obj.value("login").toString(fallbackLogin);
    session.firstName = obj.value("firstName").toString();
    session.lastName = obj.value("lastName").toString();
    session.fullName = obj.value("fullName").toString();
    session.groupName = obj.value("groupName").toString();
    session.email = obj.value("email").toString();
    session.phone = obj.value("phone").toString();
    if (session.fullName.trimmed().isEmpty()) {
        session.fullName = QString("%1 %2")
            .arg(session.lastName, session.firstName)
            .trimmed();
    }
    if (session.fullName.trimmed().isEmpty()) {
        session.fullName = session.login;
    }
    return session;
}

CourseData courseFromJson(const QJsonObject &obj)
{
    CourseData course;
    course.id = obj.value("id").toInt(-1);
    course.title = obj.value("title").toString();
    course.description = obj.value("description").toString();
    course.teacherId = obj.value("teacherId").toInt(-1);
    course.teacherName = obj.value("teacherName").toString();
    course.lessonsCount = obj.value("lessonsCount").toInt(0);
    course.testsCount = obj.value("testsCount").toInt(0);
    course.studentsCount = obj.value("studentsCount").toInt(0);
    course.enrolled = obj.value("enrolled").toBool(false);
    course.nearestDeadlineAt = obj.value("nearestDeadlineAt").toString();
    return course;
}

TestData testFromJson(const QJsonObject &obj, int fallbackCourseId = -1)
{
    TestData test;
    test.id = obj.value("id").toInt(-1);
    test.courseId = obj.value("courseId").toInt(fallbackCourseId);
    test.title = obj.value("title").toString();
    test.status = obj.value("status").toString("active");
    test.deadlineAt = obj.value("deadlineAt").toString();
    test.available = obj.value("available").toBool(test.status == "active");
    test.maxAttempts = obj.value("maxAttempts").toInt(0);
    test.timeLimitMinutes = obj.value("timeLimitMinutes").toInt(30);
    test.attemptsUsed = obj.value("attemptsUsed").toInt(0);
    test.passed = obj.value("passed").toBool(false);
    test.bestPercentage = obj.value("bestPercentage").toDouble(0.0);
    test.canAttempt = obj.value("canAttempt").toBool(test.available);
    return test;
}

}

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
{
}

void ApiClient::setBaseUrl(const QString &baseUrl)
{
    m_baseUrl = baseUrl;
}

void ApiClient::setToken(const QString &token)
{
    m_token = token;
}

QString ApiClient::token() const
{
    return m_token;
}

void ApiClient::checkHealth(
    QObject *context,
    std::function<void(const QString &status)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(createRequest("/api/health", false));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ health-check");
            reply->deleteLater();
            return;
        }

        onSuccess(doc.object().value("status").toString("ok"));
        reply->deleteLater();
    });
}

void ApiClient::login(
    const QString &login,
    const QString &password,
    QObject *context,
    std::function<void(const SessionData &session)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["login"] = login;
    body["password"] = password;

    QNetworkRequest request = createRequest("/api/auth/login", false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [this, reply, login, onSuccess = std::move(onSuccess), onError = std::move(onError)]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ сервера");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        const QString token = obj.value("token").toString();
        if (token.isEmpty()) {
            onError(extractErrorMessage(data, "Не удалось выполнить вход"));
            reply->deleteLater();
            return;
        }

        SessionData session = sessionFromJson(obj, login);

        m_token = token;
        onSuccess(session);
        reply->deleteLater();
    });
}

void ApiClient::registerStudent(
    const QString &login,
    const QString &password,
    const QString &firstName,
    const QString &lastName,
    const QString &groupName,
    const QString &email,
    const QString &phone,
    QObject *context,
    std::function<void(const SessionData &session)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["login"] = login;
    body["password"] = password;
    body["firstName"] = firstName;
    body["lastName"] = lastName;
    body["groupName"] = groupName;
    body["email"] = email;
    body["phone"] = phone;

    QNetworkRequest request = createRequest("/api/auth/register", false);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [this, reply, login, onSuccess = std::move(onSuccess), onError = std::move(onError)]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ регистрации");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        const QString token = obj.value("token").toString();
        if (token.isEmpty()) {
            onError(extractErrorMessage(data, "Не удалось зарегистрировать студента"));
            reply->deleteLater();
            return;
        }

        SessionData session = sessionFromJson(obj, login);
        m_token = token;
        onSuccess(session);
        reply->deleteLater();
    });
}

void ApiClient::getCurrentProfile(
    QObject *context,
    std::function<void(const SessionData &session)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(createRequest("/api/auth/me"));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ профиля");
            reply->deleteLater();
            return;
        }

        SessionData session = sessionFromJson(doc.object());
        session.token = m_token;
        onSuccess(session);
        reply->deleteLater();
    });
}

void ApiClient::changePassword(
    const QString &oldPassword,
    const QString &newPassword,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["oldPassword"] = oldPassword;
    body["newPassword"] = newPassword;

    QNetworkRequest request = createRequest("/api/auth/change-password");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::getCourses(
    QObject *context,
    std::function<void(const QVector<CourseData> &courses)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(createRequest("/api/courses/all"));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ сервера");
            reply->deleteLater();
            return;
        }

        QVector<CourseData> courses;
        for (const auto &value : doc.object().value("courses").toArray()) {
            const QJsonObject obj = value.toObject();
            courses.push_back(courseFromJson(obj));
        }

        onSuccess(courses);
        reply->deleteLater();
    });
}

void ApiClient::createCourse(
    const QString &title,
    const QString &description,
    QObject *context,
    std::function<void(const CourseData &course)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["description"] = description;

    QNetworkRequest request = createRequest("/api/courses");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после создания курса");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        CourseData course = courseFromJson(obj);
        onSuccess(course);
        reply->deleteLater();
    });
}

void ApiClient::updateCourse(
    int courseId,
    const QString &title,
    const QString &description,
    QObject *context,
    std::function<void(const CourseData &course)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["description"] = description;

    QNetworkRequest request = createRequest(QString("/api/courses/%1").arg(courseId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после обновления курса");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        CourseData course = courseFromJson(obj);
        onSuccess(course);
        reply->deleteLater();
    });
}

void ApiClient::deleteCourse(
    int courseId,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.deleteResource(
        createRequest(QString("/api/courses/%1").arg(courseId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::enrollCourse(
    int courseId,
    QObject *context,
    std::function<void(const QString &message)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.post(
        createRequest(QString("/api/courses/%1/enroll").arg(courseId)),
        QByteArray());

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        const QString message = doc.isObject()
            ? doc.object().value("message").toString("Запись на курс прошла успешно")
            : QString("Запись на курс прошла успешно");

        onSuccess(message);
        reply->deleteLater();
    });
}

void ApiClient::getLessons(
    int courseId,
    QObject *context,
    std::function<void(const QVector<LessonData> &lessons)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/courses/%1/lessons").arg(courseId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по урокам");
            reply->deleteLater();
            return;
        }

        QVector<LessonData> lessons;
        for (const auto &value : doc.object().value("lessons").toArray()) {
            const QJsonObject obj = value.toObject();
            LessonData lesson;
            lesson.id = obj.value("id").toInt(-1);
            lesson.courseId = obj.value("courseId").toInt(-1);
            lesson.title = obj.value("title").toString();
            lesson.content = obj.value("content").toString();
            lesson.completed = obj.value("completed").toBool(false);
            lessons.push_back(lesson);
        }

        onSuccess(lessons);
        reply->deleteLater();
    });
}

void ApiClient::createLesson(
    int courseId,
    const QString &title,
    const QString &content,
    QObject *context,
    std::function<void(const LessonData &lesson)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["content"] = content;

    QNetworkRequest request = createRequest(QString("/api/courses/%1/lessons").arg(courseId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после создания урока");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        LessonData lesson;
        lesson.id = obj.value("id").toInt(-1);
        lesson.courseId = obj.value("courseId").toInt(-1);
        lesson.title = obj.value("title").toString();
        lesson.content = obj.value("content").toString();
        lesson.completed = obj.value("completed").toBool(false);
        onSuccess(lesson);
        reply->deleteLater();
    });
}

void ApiClient::updateLesson(
    int lessonId,
    const QString &title,
    const QString &content,
    QObject *context,
    std::function<void(const LessonData &lesson)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["content"] = content;

    QNetworkRequest request = createRequest(QString("/api/lessons/%1").arg(lessonId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после обновления урока");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        LessonData lesson;
        lesson.id = obj.value("id").toInt(-1);
        lesson.courseId = obj.value("courseId").toInt(-1);
        lesson.title = obj.value("title").toString();
        lesson.content = obj.value("content").toString();
        lesson.completed = obj.value("completed").toBool(false);
        onSuccess(lesson);
        reply->deleteLater();
    });
}

void ApiClient::deleteLesson(
    int lessonId,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.deleteResource(
        createRequest(QString("/api/lessons/%1").arg(lessonId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::markLessonCompleted(
    int lessonId,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.post(
        createRequest(QString("/api/lessons/%1/complete").arg(lessonId)),
        QByteArray());

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::getMaterials(
    int lessonId,
    QObject *context,
    std::function<void(const QVector<MaterialData> &materials)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/lessons/%1/materials").arg(lessonId), false));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по материалам");
            reply->deleteLater();
            return;
        }

        QVector<MaterialData> materials;
        for (const auto &value : doc.object().value("materials").toArray()) {
            const QJsonObject obj = value.toObject();
            MaterialData material;
            material.id = obj.value("id").toInt(-1);
            material.lessonId = obj.value("lessonId").toInt(-1);
            material.title = obj.value("title").toString();
            material.type = obj.value("type").toString();
            material.content = obj.value("content").toString();
            materials.push_back(material);
        }

        onSuccess(materials);
        reply->deleteLater();
    });
}

void ApiClient::getCourseMaterials(
    int courseId,
    QObject *context,
    std::function<void(const QVector<MaterialData> &materials)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/courses/%1/materials").arg(courseId), false));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по материалам курса");
            reply->deleteLater();
            return;
        }

        QVector<MaterialData> materials;
        for (const auto &value : doc.object().value("materials").toArray()) {
            const QJsonObject obj = value.toObject();
            MaterialData material;
            material.id = obj.value("id").toInt(-1);
            material.lessonId = obj.value("lessonId").toInt(-1);
            material.title = obj.value("title").toString();
            material.type = obj.value("type").toString();
            material.content = obj.value("content").toString();
            materials.push_back(material);
        }

        onSuccess(materials);
        reply->deleteLater();
    });
}

void ApiClient::createMaterial(
    int lessonId,
    const QString &title,
    const QString &type,
    const QString &content,
    QObject *context,
    std::function<void(const MaterialData &material)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["type"] = type;
    body["content"] = content;

    QNetworkRequest request = createRequest(QString("/api/lessons/%1/materials").arg(lessonId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после создания материала");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        MaterialData material;
        material.id = obj.value("id").toInt(-1);
        material.lessonId = obj.value("lessonId").toInt(-1);
        material.title = obj.value("title").toString();
        material.type = obj.value("type").toString();
        material.content = obj.value("content").toString();
        onSuccess(material);
        reply->deleteLater();
    });
}

void ApiClient::updateMaterial(
    int materialId,
    const QString &title,
    const QString &type,
    const QString &content,
    QObject *context,
    std::function<void(const MaterialData &material)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["type"] = type;
    body["content"] = content;

    QNetworkRequest request = createRequest(QString("/api/materials/%1").arg(materialId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после обновления материала");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        MaterialData material;
        material.id = obj.value("id").toInt(-1);
        material.lessonId = obj.value("lessonId").toInt(-1);
        material.title = obj.value("title").toString();
        material.type = obj.value("type").toString();
        material.content = obj.value("content").toString();
        onSuccess(material);
        reply->deleteLater();
    });
}

void ApiClient::deleteMaterial(
    int materialId,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.deleteResource(
        createRequest(QString("/api/materials/%1").arg(materialId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::downloadMaterialFile(
    int materialId,
    QObject *context,
    std::function<void(const MaterialFileData &file)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/materials/%1/download").arg(materialId)));

    connect(reply, &QNetworkReply::finished, context, [reply, materialId, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        MaterialFileData file;
        file.materialId = materialId;
        file.fileName = fileNameFromContentDisposition(
            reply->rawHeader("Content-Disposition"),
            materialId);
        file.mimeType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        file.bytes = data;

        onSuccess(file);
        reply->deleteLater();
    });
}

void ApiClient::getTests(
    int courseId,
    QObject *context,
    std::function<void(const QVector<TestData> &tests)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/courses/%1/tests").arg(courseId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this, courseId]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по тестам");
            reply->deleteLater();
            return;
        }

        QVector<TestData> tests;
        for (const auto &value : doc.object().value("tests").toArray()) {
            const QJsonObject obj = value.toObject();
            tests.push_back(testFromJson(obj, courseId));
        }

        onSuccess(tests);
        reply->deleteLater();
    });
}

void ApiClient::createTest(
    int courseId,
    const QString &title,
    const QString &status,
    const QString &deadlineAt,
    int maxAttempts,
    int timeLimitMinutes,
    QObject *context,
    std::function<void(const TestData &test)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["status"] = status;
    body["deadlineAt"] = deadlineAt;
    body["maxAttempts"] = maxAttempts;
    body["timeLimitMinutes"] = timeLimitMinutes;

    QNetworkRequest request = createRequest(QString("/api/courses/%1/tests").arg(courseId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после создания теста");
            reply->deleteLater();
            return;
        }

        onSuccess(testFromJson(doc.object()));
        reply->deleteLater();
    });
}

void ApiClient::updateTest(
    int testId,
    const QString &title,
    const QString &status,
    const QString &deadlineAt,
    int maxAttempts,
    int timeLimitMinutes,
    QObject *context,
    std::function<void(const TestData &test)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;
    body["status"] = status;
    body["deadlineAt"] = deadlineAt;
    body["maxAttempts"] = maxAttempts;
    body["timeLimitMinutes"] = timeLimitMinutes;

    QNetworkRequest request = createRequest(QString("/api/tests/%1").arg(testId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после обновления теста");
            reply->deleteLater();
            return;
        }

        onSuccess(testFromJson(doc.object()));
        reply->deleteLater();
    });
}

void ApiClient::deleteTest(
    int testId,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.deleteResource(
        createRequest(QString("/api/tests/%1").arg(testId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::getQuestions(
    int testId,
    QObject *context,
    std::function<void(const QVector<QuestionData> &questions)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/tests/%1/questions").arg(testId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по вопросам");
            reply->deleteLater();
            return;
        }

        QVector<QuestionData> questions;
        for (const auto &value : doc.object().value("questions").toArray()) {
            const QJsonObject obj = value.toObject();
            QuestionData question;
            question.id = obj.value("id").toInt(-1);
            question.text = obj.value("text").toString();
            question.correctAnswerId = obj.value("correctAnswerId").toInt(-1);

            for (const auto &optionValue : obj.value("options").toArray()) {
                const QJsonObject optionObject = optionValue.toObject();
                AnswerOptionData option;
                option.id = optionObject.value("id").toInt(-1);
                option.text = optionObject.value("text").toString();
                question.options.push_back(option);
            }

            questions.push_back(question);
        }

        onSuccess(questions);
        reply->deleteLater();
    });
}

void ApiClient::getManageQuestions(
    int testId,
    QObject *context,
    std::function<void(const QVector<QuestionData> &questions)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/tests/%1/questions/manage").arg(testId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по вопросам теста");
            reply->deleteLater();
            return;
        }

        QVector<QuestionData> questions;
        for (const auto &value : doc.object().value("questions").toArray()) {
            const QJsonObject obj = value.toObject();
            QuestionData question;
            question.id = obj.value("id").toInt(-1);
            question.text = obj.value("text").toString();
            question.correctAnswerId = obj.value("correctAnswerId").toInt(-1);

            for (const auto &optionValue : obj.value("options").toArray()) {
                const QJsonObject optionObject = optionValue.toObject();
                AnswerOptionData option;
                option.id = optionObject.value("id").toInt(-1);
                option.text = optionObject.value("text").toString();
                question.options.push_back(option);
            }

            questions.push_back(question);
        }

        onSuccess(questions);
        reply->deleteLater();
    });
}

void ApiClient::createQuestion(
    int testId,
    const QString &text,
    const QStringList &options,
    int correctOptionIndex,
    QObject *context,
    std::function<void(const QuestionData &question)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonArray optionsArray;
    for (const QString &option : options) {
        optionsArray.push_back(option);
    }

    QJsonObject body;
    body["text"] = text;
    body["options"] = optionsArray;
    body["correctOptionIndex"] = correctOptionIndex;

    QNetworkRequest request = createRequest(QString("/api/tests/%1/questions").arg(testId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после создания вопроса");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        QuestionData question;
        question.id = obj.value("id").toInt(-1);
        question.text = obj.value("text").toString();
        question.correctAnswerId = obj.value("correctAnswerId").toInt(-1);

        for (const auto &optionValue : obj.value("options").toArray()) {
            const QJsonObject optionObject = optionValue.toObject();
            AnswerOptionData option;
            option.id = optionObject.value("id").toInt(-1);
            option.text = optionObject.value("text").toString();
            question.options.push_back(option);
        }

        onSuccess(question);
        reply->deleteLater();
    });
}

void ApiClient::updateQuestion(
    int questionId,
    const QString &text,
    const QStringList &options,
    int correctOptionIndex,
    QObject *context,
    std::function<void(const QuestionData &question)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonArray optionsArray;
    for (const QString &option : options) {
        optionsArray.push_back(option);
    }

    QJsonObject body;
    body["text"] = text;
    body["options"] = optionsArray;
    body["correctOptionIndex"] = correctOptionIndex;

    QNetworkRequest request = createRequest(QString("/api/questions/%1").arg(questionId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после обновления вопроса");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        QuestionData question;
        question.id = obj.value("id").toInt(-1);
        question.text = obj.value("text").toString();
        question.correctAnswerId = obj.value("correctAnswerId").toInt(-1);

        for (const auto &optionValue : obj.value("options").toArray()) {
            const QJsonObject optionObject = optionValue.toObject();
            AnswerOptionData option;
            option.id = optionObject.value("id").toInt(-1);
            option.text = optionObject.value("text").toString();
            question.options.push_back(option);
        }

        onSuccess(question);
        reply->deleteLater();
    });
}

void ApiClient::deleteQuestion(
    int questionId,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.deleteResource(
        createRequest(QString("/api/questions/%1").arg(questionId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::submitTest(
    int testId,
    const QVector<QPair<int, int>> &answers,
    QObject *context,
    std::function<void(const AttemptData &result)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonArray answersArray;
    for (const auto &answer : answers) {
        QJsonObject item;
        item["questionId"] = answer.first;
        item["answerId"] = answer.second;
        answersArray.push_back(item);
    }

    QJsonObject body;
    body["answers"] = answersArray;

    QNetworkRequest request = createRequest(QString("/api/tests/%1/submit").arg(testId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после отправки");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        AttemptData result;
        result.testId = obj.value("testId").toInt(-1);
        result.courseId = obj.value("courseId").toInt(-1);
        result.score = obj.value("score").toInt();
        result.total = obj.value("total").toInt();
        result.percentage = obj.value("percentage").toDouble();
        result.passed = obj.value("passed").toBool();
        result.testTitle = obj.value("testTitle").toString();
        result.submittedAt = obj.value("submittedAt").toString();

        onSuccess(result);
        reply->deleteLater();
    });
}

void ApiClient::getAttempts(
    int userId,
    QObject *context,
    std::function<void(const QVector<AttemptData> &attempts)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/users/%1/attempts").arg(userId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по попыткам");
            reply->deleteLater();
            return;
        }

        QVector<AttemptData> attempts;
        for (const auto &value : doc.object().value("attempts").toArray()) {
            const QJsonObject obj = value.toObject();
            AttemptData attempt;
            attempt.testId = obj.value("testId").toInt(-1);
            attempt.courseId = obj.value("courseId").toInt(-1);
            attempt.score = obj.value("score").toInt();
            attempt.total = obj.value("total").toInt();
            attempt.percentage = obj.value("percentage").toDouble();
            attempt.passed = obj.value("passed").toBool();
            attempt.testTitle = obj.value("testTitle").toString();
            attempt.submittedAt = obj.value("submittedAt").toString();
            attempts.push_back(attempt);
        }

        onSuccess(attempts);
        reply->deleteLater();
    });
}

void ApiClient::getAdminOverview(
    QObject *context,
    std::function<void(const AdminOverviewData &overview)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(createRequest("/api/admin/overview"));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по admin-обзору");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        AdminOverviewData overview;
        overview.totalUsers = obj.value("totalUsers").toInt(0);
        overview.studentsCount = obj.value("studentsCount").toInt(0);
        overview.teachersCount = obj.value("teachersCount").toInt(0);
        overview.adminsCount = obj.value("adminsCount").toInt(0);
        overview.coursesCount = obj.value("coursesCount").toInt(0);
        overview.lessonsCount = obj.value("lessonsCount").toInt(0);
        overview.testsCount = obj.value("testsCount").toInt(0);
        overview.enrollmentsCount = obj.value("enrollmentsCount").toInt(0);
        onSuccess(overview);
        reply->deleteLater();
    });
}

void ApiClient::getAdminUsers(
    QObject *context,
    std::function<void(const QVector<AdminUserData> &users)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(createRequest("/api/admin/users"));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по пользователям системы");
            reply->deleteLater();
            return;
        }

        QVector<AdminUserData> users;
        for (const auto &value : doc.object().value("users").toArray()) {
            const QJsonObject obj = value.toObject();
            AdminUserData user;
            user.id = obj.value("id").toInt(-1);
            user.login = obj.value("login").toString();
            user.role = obj.value("role").toString();
            user.firstName = obj.value("firstName").toString();
            user.lastName = obj.value("lastName").toString();
            user.groupName = obj.value("groupName").toString();
            user.email = obj.value("email").toString();
            user.phone = obj.value("phone").toString();
            users.push_back(user);
        }

        onSuccess(users);
        reply->deleteLater();
    });
}

void ApiClient::getAdminGroups(
    QObject *context,
    std::function<void(const QStringList &groups)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(createRequest("/api/admin/groups"));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по группам");
            reply->deleteLater();
            return;
        }

        QStringList groups;
        for (const auto &value : doc.object().value("groups").toArray()) {
            groups.push_back(value.toString());
        }

        onSuccess(groups);
        reply->deleteLater();
    });
}

void ApiClient::createAdminGroup(
    const QString &groupName,
    QObject *context,
    std::function<void(const QString &groupName)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["name"] = groupName;

    QNetworkRequest request = createRequest("/api/admin/groups");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, groupName, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        const QString created = doc.isObject()
            ? doc.object().value("name").toString(groupName)
            : groupName;
        onSuccess(created);
        reply->deleteLater();
    });
}

void ApiClient::renameAdminGroup(
    const QString &oldName,
    const QString &newName,
    QObject *context,
    std::function<void(const QString &groupName)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["oldName"] = oldName;
    body["newName"] = newName;

    QNetworkRequest request = createRequest("/api/admin/groups");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, newName, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        const QString renamed = doc.isObject()
            ? doc.object().value("name").toString(newName)
            : newName;
        onSuccess(renamed);
        reply->deleteLater();
    });
}

void ApiClient::deleteAdminGroup(
    const QString &groupName,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["name"] = groupName;

    QNetworkRequest request = createRequest("/api/admin/groups");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.sendCustomRequest(
        request,
        "DELETE",
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::getAdminAudit(
    QObject *context,
    std::function<void(const QVector<AdminAuditEventData> &events)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(createRequest("/api/admin/audit?limit=8"));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по журналу действий");
            reply->deleteLater();
            return;
        }

        QVector<AdminAuditEventData> events;
        for (const auto &value : doc.object().value("events").toArray()) {
            const QJsonObject obj = value.toObject();
            AdminAuditEventData event;
            event.id = obj.value("id").toInt(-1);
            event.adminId = obj.value("adminId").toInt(-1);
            event.adminLogin = obj.value("adminLogin").toString();
            event.action = obj.value("action").toString();
            event.targetType = obj.value("targetType").toString();
            event.targetId = obj.value("targetId").toInt(-1);
            event.details = obj.value("details").toString();
            event.createdAt = obj.value("createdAt").toString();
            events.push_back(event);
        }

        onSuccess(events);
        reply->deleteLater();
    });
}

void ApiClient::createAdminUser(
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
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["login"] = login;
    body["password"] = password;
    body["role"] = role;
    body["firstName"] = firstName;
    body["lastName"] = lastName;
    body["groupName"] = groupName;
    body["email"] = email;
    body["phone"] = phone;

    QNetworkRequest request = createRequest("/api/admin/users");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после создания пользователя");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        AdminUserData user;
        user.id = obj.value("userId").toInt(-1);
        user.login = obj.value("login").toString();
        user.role = obj.value("role").toString();
        user.firstName = obj.value("firstName").toString();
        user.lastName = obj.value("lastName").toString();
        user.groupName = obj.value("groupName").toString();
        user.email = obj.value("email").toString();
        user.phone = obj.value("phone").toString();
        onSuccess(user);
        reply->deleteLater();
    });
}

void ApiClient::updateAdminUserRole(
    int userId,
    const QString &role,
    QObject *context,
    std::function<void(const AdminUserData &user)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["role"] = role;

    QNetworkRequest request = createRequest(QString("/api/admin/users/%1").arg(userId));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_networkManager.put(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ после обновления роли");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        AdminUserData user;
        user.id = obj.value("id").toInt(-1);
        user.login = obj.value("login").toString();
        user.role = obj.value("role").toString();
        user.editable = true;
        onSuccess(user);
        reply->deleteLater();
    });
}

void ApiClient::deleteAdminUser(
    int userId,
    QObject *context,
    std::function<void()> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.deleteResource(
        createRequest(QString("/api/admin/users/%1").arg(userId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        onSuccess();
        reply->deleteLater();
    });
}

void ApiClient::getCourseStudents(
    int courseId,
    QObject *context,
    std::function<void(const QVector<CourseStudentData> &students)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/courses/%1/students").arg(courseId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по студентам курса");
            reply->deleteLater();
            return;
        }

        QVector<CourseStudentData> students;
        for (const auto &value : doc.object().value("students").toArray()) {
            const QJsonObject obj = value.toObject();
            CourseStudentData student;
            student.id = obj.value("id").toInt(-1);
            student.login = obj.value("login").toString();
            student.groupName = obj.value("groupName").toString();
            student.progress = obj.value("progress").toInt(0);
            student.lessonProgress = obj.value("lessonProgress").toInt(0);
            student.testProgress = obj.value("testProgress").toInt(0);
            students.push_back(student);
        }

        onSuccess(students);
        reply->deleteLater();
    });
}

void ApiClient::getTeacherStudentAttempts(
    int courseId,
    int studentId,
    QObject *context,
    std::function<void(const QVector<TeacherStudentAttemptData> &attempts)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/courses/%1/students/%2/attempts").arg(courseId).arg(studentId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по попыткам студента");
            reply->deleteLater();
            return;
        }

        QVector<TeacherStudentAttemptData> attempts;
        for (const auto &value : doc.object().value("attempts").toArray()) {
            const QJsonObject obj = value.toObject();
            TeacherStudentAttemptData attempt;
            attempt.testTitle = obj.value("testTitle").toString();
            attempt.score = obj.value("score").toInt(0);
            attempt.total = obj.value("total").toInt(0);
            attempt.percentage = obj.value("percentage").toDouble(0.0);
            attempt.passed = obj.value("passed").toBool(false);
            attempts.push_back(attempt);
        }

        onSuccess(attempts);
        reply->deleteLater();
    });
}

void ApiClient::getCourseAnalytics(
    int courseId,
    QObject *context,
    std::function<void(const TeacherCourseAnalyticsData &analytics)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QNetworkReply *reply = m_networkManager.get(
        createRequest(QString("/api/courses/%1/analytics").arg(courseId)));

    connect(reply, &QNetworkReply::finished, context, [reply, onSuccess = std::move(onSuccess), onError = std::move(onError), this]() {
        const QByteArray data = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            onError(extractErrorMessage(data, reply->errorString()));
            reply->deleteLater();
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) {
            onError("Некорректный ответ по аналитике курса");
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = doc.object();
        TeacherCourseAnalyticsData analytics;
        analytics.studentsCount = obj.value("studentsCount").toInt(0);
        analytics.attemptsCount = obj.value("attemptsCount").toInt(0);
        analytics.averagePercentage = obj.value("averagePercentage").toDouble(0.0);
        analytics.averageLessonProgress = obj.value("averageLessonProgress").toDouble(0.0);

        for (const auto &value : obj.value("results").toArray()) {
            const QJsonObject rowObj = value.toObject();
            TeacherAnalyticsRowData row;
            row.studentLogin = rowObj.value("studentLogin").toString();
            row.testTitle = rowObj.value("testTitle").toString();
            row.score = rowObj.value("score").toInt(0);
            row.total = rowObj.value("total").toInt(0);
            row.percentage = rowObj.value("percentage").toDouble(0.0);
            row.passed = rowObj.value("passed").toBool(false);
            analytics.rows.push_back(row);
        }

        onSuccess(analytics);
        reply->deleteLater();
    });
}

QNetworkRequest ApiClient::createRequest(const QString &path, bool withAuth) const
{
    QNetworkRequest request{QUrl(m_baseUrl + path)};
    if (withAuth && !m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer " + m_token).toUtf8());
    }
    return request;
}

QString ApiClient::extractErrorMessage(const QByteArray &responseData, const QString &fallback) const
{
    const QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (doc.isObject()) {
        const QString error = doc.object().value("error").toString();
        if (!error.isEmpty()) {
            return error;
        }
    }
    return fallback;
}
