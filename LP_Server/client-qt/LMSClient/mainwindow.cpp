#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked,
            this, &MainWindow::onLoginClicked); // привязка функции авторизации к кнопке

    ui->statusLabel->setText("Not logged in");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onLoginClicked()
{
    // чтение логина и пароля из соответствующих полей
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("Enter login and password");
        return;
    }

    // создание json объекта
    QJsonObject body;
    body["login"] = login;
    body["password"] = password;

    // перевод json в байты для отправки по HTTP
    QJsonDocument jsonDoc(body);
    QByteArray jsonData = jsonDoc.toJson();

    // создание запроса на сервер, эндпоинт логина
    QNetworkRequest request(QUrl("http://localhost:8080/api/auth/login"));
    // объявляем что отправка для REST API (JSON)
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // отправка POST запроса
    QNetworkReply *reply = networkManager->post(request, jsonData);

    ui->statusLabel->setText("Logging in...");

    // ждем ответ от сервера
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        QByteArray responseData = reply->readAll();

        // проверка сетевой ошибки
        if (reply->error() != QNetworkReply::NoError) {
            ui->statusLabel->setText("Network error");
            qDebug() << "Network error:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        // преобразуем ответ сервера в JSON
        QJsonDocument responseJson = QJsonDocument::fromJson(responseData);

        // проверка на  JSON или нет
        if (!responseJson.isObject()) {
            ui->statusLabel->setText("Invalid server response");
            reply->deleteLater();
            return;
        }

        // получаем объект и проверяем токен
        QJsonObject obj = responseJson.object();

        if (!obj.contains("token")) {
            ui->statusLabel->setText("Login failed");
            qDebug() << "Response:" << responseData;
            reply->deleteLater();
            return;
        }

        authToken = obj["token"].toString();

        ui->statusLabel->setText("Logged in");
        qDebug() << "JWT token:" << authToken;

        reply->deleteLater();
    });
}
