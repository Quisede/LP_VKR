#include "apiclient.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

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

        SessionData session;
        session.token = token;
        session.userId = obj.value("userId").toInt(-1);
        session.role = obj.value("role").toString();
        session.login = login;

        m_token = token;
        onSuccess(session);
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
            CourseData course;
            course.id = obj.value("id").toInt(-1);
            course.title = obj.value("title").toString();
            course.description = obj.value("description").toString();
            course.teacherId = obj.value("teacherId").toInt(-1);
            courses.push_back(course);
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
        CourseData course;
        course.id = obj.value("id").toInt(-1);
        course.title = obj.value("title").toString();
        course.description = obj.value("description").toString();
        course.teacherId = obj.value("teacherId").toInt(-1);
        onSuccess(course);
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
        onSuccess(lesson);
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
            TestData test;
            test.id = obj.value("id").toInt(-1);
            test.courseId = courseId;
            test.title = obj.value("title").toString();
            tests.push_back(test);
        }

        onSuccess(tests);
        reply->deleteLater();
    });
}

void ApiClient::createTest(
    int courseId,
    const QString &title,
    QObject *context,
    std::function<void(const TestData &test)> onSuccess,
    std::function<void(const QString &error)> onError)
{
    QJsonObject body;
    body["title"] = title;

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

        const QJsonObject obj = doc.object();
        TestData test;
        test.id = obj.value("id").toInt(-1);
        test.courseId = obj.value("courseId").toInt(-1);
        test.title = obj.value("title").toString();
        onSuccess(test);
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
        createRequest(QString("/api/tests/%1/questions").arg(testId), false));

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
        result.score = obj.value("score").toInt();
        result.total = obj.value("total").toInt();
        result.percentage = obj.value("percentage").toDouble();
        result.passed = obj.value("passed").toBool();

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
            attempt.score = obj.value("score").toInt();
            attempt.total = obj.value("total").toInt();
            attempt.percentage = obj.value("percentage").toDouble();
            attempt.passed = obj.value("passed").toBool();
            attempts.push_back(attempt);
        }

        onSuccess(attempts);
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
            student.progress = obj.value("progress").toInt(0);
            students.push_back(student);
        }

        onSuccess(students);
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
