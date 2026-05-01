#include "loginwindow.h"
#include "ui_loginwindow.h"

#include "../api/apiclient.h"

#include <QDebug>

LoginWindow::LoginWindow(ApiClient *apiClient, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
    , m_apiClient(apiClient)
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

void LoginWindow::resetState()
{
    ui->passwordEdit->clear();
    ui->statusLabel->setText("Введите данные для входа");
}

void LoginWindow::onLoginClicked()
{
    const QString login = ui->loginEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();

    if (login.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("Введите логин и пароль");
        return;
    }

    ui->loginButton->setEnabled(false);
    ui->statusLabel->setText("Выполняем вход...");

    m_apiClient->login(
        login,
        password,
        this,
        [this](const SessionData &session) {
            ui->loginButton->setEnabled(true);
            ui->statusLabel->setText("Успешный вход");
            qDebug() << "JWT token:" << session.token;
            emit loginSuccess(session);
        },
        [this](const QString &error) {
            ui->loginButton->setEnabled(true);
            ui->statusLabel->setText(error);
            qDebug() << "Login error:" << error;
        });
}
