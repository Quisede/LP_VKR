#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setWindowTitle("LMS Client");

    connect(ui->loadCoursesButton, &QPushButton::clicked,
            this, &MainWindow::onLoadCoursesClicked);

    connect(ui->enrollButton, &QPushButton::clicked,
            this, &MainWindow::onEnrollClicked);

    connect(ui->loadTestButton, &QPushButton::clicked,
            this, &MainWindow::onLoadTestClicked);

    connect(ui->submitTestButton, &QPushButton::clicked,
            this, &MainWindow::onSubmitTestClicked);

    connect(ui->coursesButton, &QPushButton::clicked,
            this, &MainWindow::showCoursesPage);

    connect(ui->testButton, &QPushButton::clicked,
            this, &MainWindow::showTestPage);

    ui->stackedWidget->setCurrentWidget(ui->coursesPage);
    ui->statusLabel->setText("Выбери раздел слева и начни работу");
    ui->questionLabel->setText("Здесь появится вопрос после загрузки теста");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoadCoursesClicked()
{
    if (authToken.isEmpty()) {
        ui->statusLabel->setText("Сначала войди в систему");
        return;
    }

    QNetworkRequest request{QUrl("http://localhost:8080/api/courses/all")};

    request.setRawHeader(
        "Authorization",
        QString("Bearer " + authToken).toUtf8()
        );

    QNetworkReply *reply = networkManager->get(request);

    ui->statusLabel->setText("Загружаем курсы...");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray response = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            ui->statusLabel->setText("Не удалось загрузить курсы");
            qDebug() << "Network error:" << reply->errorString();
            qDebug() << "Response:" << response;
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(response);

        if (!doc.isObject()) {
            ui->statusLabel->setText("Некорректный ответ сервера");
            qDebug() << "Response is not JSON object:" << response;
            reply->deleteLater();
            return;
        }

        QJsonObject obj = doc.object();

        if (obj.contains("error")) {
            ui->statusLabel->setText(obj["error"].toString());
            qDebug() << "Server error:" << response;
            reply->deleteLater();
            return;
        }

        if (!obj.contains("courses") || !obj["courses"].isArray()) {
            ui->statusLabel->setText("Курсы не найдены");
            qDebug() << "Missing courses array:" << response;
            reply->deleteLater();
            return;
        }

        QJsonArray courses = obj["courses"].toArray();

        ui->coursesList->clear();

        for (const auto &c : std::as_const(courses)) {
            if (!c.isObject()) {
                continue;
            }

            QJsonObject course = c.toObject();
            int id = course["id"].toInt();
            QString title = course["title"].toString();

            QListWidgetItem* item = new QListWidgetItem(title);
            item->setData(Qt::UserRole, id);

            ui->coursesList->addItem(item);
        }

        ui->statusLabel->setText("Список курсов обновлён");
        reply->deleteLater();
    });
}

void MainWindow::onEnrollClicked()
{
    if (authToken.isEmpty()) {
        ui->statusLabel->setText("Сначала войди в систему");
        return;
    }

    const int courseId = currentCourseId();
    if (courseId < 0) {
        ui->statusLabel->setText("Сначала выбери курс");
        return;
    }

    QString url = QString("http://localhost:8080/api/courses/%1/enroll")
                      .arg(courseId);

    QNetworkRequest request{QUrl(url)};
    request.setRawHeader(
        "Authorization",
        QString("Bearer " + authToken).toUtf8()
        );

    QNetworkReply *reply = networkManager->post(request, QByteArray());

    ui->statusLabel->setText("Записываем на курс...");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray response = reply->readAll();

        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.isObject() ? doc.object() : QJsonObject();

        if (reply->error() != QNetworkReply::NoError) {
            if (obj.contains("error")) {
                ui->statusLabel->setText(obj["error"].toString());
            } else {
                ui->statusLabel->setText("Не удалось записаться на курс");
            }

            qDebug() << "Enroll error:" << reply->errorString();
            qDebug() << "Response:" << response;
            reply->deleteLater();
            return;
        }

        if (obj.contains("message")) {
            ui->statusLabel->setText(obj["message"].toString());
        } else {
            ui->statusLabel->setText("Запись на курс прошла успешно");
        }

        onLoadCoursesClicked();

        reply->deleteLater();
    });
}

void MainWindow::onLoadTestClicked()
{
    if (authToken.isEmpty()) {
        ui->statusLabel->setText("Сначала войди в систему");
        return;
    }

    const int courseId = currentCourseId();
    if (courseId < 0) {
        ui->statusLabel->setText("Выбери курс перед загрузкой теста");
        return;
    }

    QNetworkRequest request{
        QUrl(QString("http://localhost:8080/api/courses/%1/tests").arg(courseId))
    };

    request.setRawHeader("Authorization",
                         QString("Bearer " + authToken).toUtf8());

    QNetworkReply* reply = networkManager->get(request);

    ui->statusLabel->setText("Загружаем тест...");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (reply->error() != QNetworkReply::NoError) {
            ui->statusLabel->setText("Не удалось загрузить тест");
            qDebug() << "Load test error:" << reply->errorString();
            qDebug() << "Response:" << data;
            reply->deleteLater();
            return;
        }

        if (!doc.isObject()) {
            ui->statusLabel->setText("Некорректный ответ по тесту");
            reply->deleteLater();
            return;
        }

        QJsonArray tests = doc.object()["tests"].toArray();

        if (tests.isEmpty()) {
            ui->statusLabel->setText("Для этого курса тестов пока нет");
            reply->deleteLater();
            return;
        }

        currentTestId = tests[0].toObject()["id"].toInt();

        QNetworkRequest qreq{
            QUrl(QString("http://localhost:8080/api/tests/%1/questions")
                     .arg(currentTestId))
        };

        qreq.setRawHeader("Authorization",
                          QString("Bearer " + authToken).toUtf8());

        QNetworkReply* qreply = networkManager->get(qreq);

        connect(qreply, &QNetworkReply::finished, this, [this, qreply]() {

            QByteArray qdata = qreply->readAll();
            QJsonDocument qdoc = QJsonDocument::fromJson(qdata);

            if (qreply->error() != QNetworkReply::NoError) {
                ui->statusLabel->setText("Не удалось загрузить вопросы");
                qDebug() << "Load questions error:" << qreply->errorString();
                qDebug() << "Response:" << qdata;
                qreply->deleteLater();
                return;
            }

            if (!qdoc.isObject()) {
                ui->statusLabel->setText("Некорректный ответ по вопросам");
                qreply->deleteLater();
                return;
            }

            QJsonArray questions = qdoc.object()["questions"].toArray();

            if (questions.isEmpty()) {
                ui->statusLabel->setText("В тесте нет вопросов");
                qreply->deleteLater();
                return;
            }

            QJsonObject q = questions[0].toObject();

            currentQuestionId = q["id"].toInt();

            ui->questionLabel->setText(q["text"].toString());

            ui->answersBox->clear();

            QJsonArray options = q["options"].toArray();

            for (const auto& opt : options) {
                QJsonObject o = opt.toObject();

                int id = o["id"].toInt();
                QString text = o["text"].toString();

                ui->answersBox->addItem(text, id);
            }

            ui->statusLabel->setText("Тест загружен");
            qreply->deleteLater();
        });

        reply->deleteLater();
    });
}

void MainWindow::onSubmitTestClicked()
{
    if (authToken.isEmpty()) {
        ui->statusLabel->setText("Сначала войди в систему");
        return;
    }

    if (currentTestId < 0 || currentQuestionId < 0) {
        ui->statusLabel->setText("Сначала загрузи тест");
        return;
    }

    const int answerId = ui->answersBox->currentData().toInt();

    QJsonObject answer;
    answer["questionId"] = currentQuestionId;
    answer["answerId"] = answerId;

    QJsonArray arr;
    arr.append(answer);

    QJsonObject body;
    body["answers"] = arr;

    QNetworkRequest request{
        QUrl(QString("http://localhost:8080/api/tests/%1/submit")
                 .arg(currentTestId))
    };

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    request.setRawHeader("Authorization",
                         QString("Bearer " + authToken).toUtf8());

    QNetworkReply* reply =
        networkManager->post(request, QJsonDocument(body).toJson());

    ui->statusLabel->setText("Отправляем ответ...");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {

        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);

        if (reply->error() != QNetworkReply::NoError) {
            ui->statusLabel->setText("Не удалось отправить ответ");
            qDebug() << "Submit test error:" << reply->errorString();
            qDebug() << "Response:" << data;
            reply->deleteLater();
            return;
        }

        if (!doc.isObject()) {
            ui->statusLabel->setText("Некорректный ответ после отправки");
            reply->deleteLater();
            return;
        }

        QJsonObject obj = doc.object();

        const int score = obj["score"].toInt();
        const int total = obj["total"].toInt();

        ui->statusLabel->setText(
            QString("Результат: %1/%2").arg(score).arg(total));
        reply->deleteLater();
    });
}

void MainWindow::setToken(const QString& token)
{
    authToken = token;
    ui->statusLabel->setText("Ты в системе. Можно загружать курсы");
}

void MainWindow::showCoursesPage()
{
    ui->stackedWidget->setCurrentWidget(ui->coursesPage);
    ui->statusLabel->setText("Раздел курсов открыт");
}

void MainWindow::showTestPage()
{
    ui->stackedWidget->setCurrentWidget(ui->testPage);
    ui->statusLabel->setText("Раздел тестов открыт");
}

int MainWindow::currentCourseId() const
{
    const QListWidgetItem *item = ui->coursesList->currentItem();
    if (!item) {
        return -1;
    }

    return item->data(Qt::UserRole).toInt();
}
