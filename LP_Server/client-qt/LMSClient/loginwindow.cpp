#include "loginwindow.h"
#include "ui_loginwindow.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
    , networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    setWindowTitle("LMS Client");
    setFixedSize(size());

    connect(ui->loginButton, &QPushButton::clicked,
            this, &LoginWindow::onLoginClicked);
    connect(ui->passwordEdit, &QLineEdit::returnPressed,
            this, &LoginWindow::onLoginClicked);
    connect(ui->loginEdit, &QLineEdit::returnPressed,
            ui->passwordEdit, qOverload<>(&QWidget::setFocus));
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::onLoginClicked()
{
    const QString login = ui->loginEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("Введите логин и пароль");
        return;
    }

    QJsonObject body;
    body["login"] = login;
    body["password"] = password;

    QNetworkRequest request{QUrl("http://localhost:8080/api/auth/login")};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager->post(
        request,
        QJsonDocument(body).toJson(QJsonDocument::Compact));

    ui->loginButton->setEnabled(false);
    ui->statusLabel->setText("Выполняем вход...");

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray responseData = reply->readAll();
        ui->loginButton->setEnabled(true);

        if (reply->error() != QNetworkReply::NoError) {
            ui->statusLabel->setText("Ошибка сети");
            qDebug() << "Network error:" << reply->errorString();
            qDebug() << "Response:" << responseData;
            reply->deleteLater();
            return;
        }

        const QJsonDocument responseJson = QJsonDocument::fromJson(responseData);
        if (!responseJson.isObject()) {
            ui->statusLabel->setText("Некорректный ответ сервера");
            qDebug() << "Invalid login response:" << responseData;
            reply->deleteLater();
            return;
        }

        const QJsonObject obj = responseJson.object();
        const QString token = obj["token"].toString();

        if (token.isEmpty()) {
            if (obj.contains("error")) {
                ui->statusLabel->setText(obj["error"].toString());
            } else {
                ui->statusLabel->setText("Не удалось войти");
            }
            qDebug() << "Login response:" << responseData;
            reply->deleteLater();
            return;
        }

        ui->statusLabel->setText("Успешный вход");
        qDebug() << "JWT token:" << token;

        emit loginSuccess(token);
        reply->deleteLater();
    });
}
