#include "loginwindow.h"
#include "ui_loginwindow.h"

#include "../api/apiclient.h"

#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

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

    auto *registerButton = new QPushButton("Зарегистрироваться как студент", this);
    registerButton->setObjectName("registerButton");
    registerButton->setStyleSheet(
        "QPushButton#registerButton {"
        " min-height: 42px;"
        " border: 1px solid #cbd5e1;"
        " border-radius: 14px;"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " font-size: 14px;"
        " font-weight: 700;"
        "}"
        "QPushButton#registerButton:hover { background-color: #eff6ff; border-color: #93c5fd; }");
    ui->cardLayout->insertWidget(ui->cardLayout->indexOf(ui->statusLabel), registerButton);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
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

void LoginWindow::onRegisterClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Регистрация студента");
    dialog.resize(520, 520);
    dialog.setStyleSheet(
        "QDialog { background: #f8fbff; }"
        "QLabel { color: #334155; font-size: 13px; font-weight: 700; }"
        "QLineEdit { background: #ffffff; color: #0f172a; border: 1px solid #dbe4f0; border-radius: 12px; padding: 10px 12px; }"
        "QLineEdit:focus { border-color: #2563eb; }");

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(22, 22, 22, 18);
    layout->setSpacing(14);

    auto *hintLabel = new QLabel("Создаётся студенческая учётная запись. После регистрации вход будет выполнен автоматически.", &dialog);
    hintLabel->setWordWrap(true);
    hintLabel->setStyleSheet("color: #475569; font-size: 13px; font-weight: 600;");

    auto *form = new QFormLayout();
    form->setHorizontalSpacing(16);
    form->setVerticalSpacing(10);

    auto *loginEdit = new QLineEdit(&dialog);
    auto *passwordEdit = new QLineEdit(&dialog);
    auto *repeatPasswordEdit = new QLineEdit(&dialog);
    auto *firstNameEdit = new QLineEdit(&dialog);
    auto *lastNameEdit = new QLineEdit(&dialog);
    auto *groupEdit = new QLineEdit(&dialog);
    auto *emailEdit = new QLineEdit(&dialog);
    auto *phoneEdit = new QLineEdit(&dialog);

    passwordEdit->setEchoMode(QLineEdit::Password);
    repeatPasswordEdit->setEchoMode(QLineEdit::Password);
    loginEdit->setPlaceholderText("student_ivanov");
    firstNameEdit->setPlaceholderText("Иван");
    lastNameEdit->setPlaceholderText("Иванов");
    groupEdit->setPlaceholderText("ИВТ-401");
    emailEdit->setPlaceholderText("student@example.com");
    phoneEdit->setPlaceholderText("+7 900 000-00-00");

    form->addRow("Логин", loginEdit);
    form->addRow("Пароль", passwordEdit);
    form->addRow("Повтор пароля", repeatPasswordEdit);
    form->addRow("Имя", firstNameEdit);
    form->addRow("Фамилия", lastNameEdit);
    form->addRow("Группа", groupEdit);
    form->addRow("Почта", emailEdit);
    form->addRow("Телефон", phoneEdit);

    auto *statusLabel = new QLabel(&dialog);
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("color: #b91c1c; font-size: 13px; font-weight: 700;");
    statusLabel->hide();

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Save)->setText("Создать аккаунт");
    buttons->button(QDialogButtonBox::Cancel)->setText("Отмена");

    layout->addWidget(hintLabel);
    layout->addLayout(form);
    layout->addWidget(statusLabel);
    layout->addStretch();
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
        const QString login = loginEdit->text().trimmed();
        const QString password = passwordEdit->text();
        const QString repeatPassword = repeatPasswordEdit->text();

        if (login.isEmpty() || password.isEmpty() || firstNameEdit->text().trimmed().isEmpty() || lastNameEdit->text().trimmed().isEmpty()) {
            statusLabel->setText("Заполните логин, пароль, имя и фамилию.");
            statusLabel->show();
            return;
        }
        if (password.size() < 6) {
            statusLabel->setText("Пароль должен содержать минимум 6 символов.");
            statusLabel->show();
            return;
        }
        if (password != repeatPassword) {
            statusLabel->setText("Пароль и повтор не совпадают.");
            statusLabel->show();
            return;
        }

        buttons->setEnabled(false);
        statusLabel->setText("Создаём аккаунт...");
        statusLabel->setStyleSheet("color: #047857; font-size: 13px; font-weight: 700;");
        statusLabel->show();

        m_apiClient->registerStudent(
            login,
            password,
            firstNameEdit->text().trimmed(),
            lastNameEdit->text().trimmed(),
            groupEdit->text().trimmed(),
            emailEdit->text().trimmed(),
            phoneEdit->text().trimmed(),
            &dialog,
            [this, &dialog](const SessionData &session) {
                dialog.accept();
                ui->statusLabel->setText("Аккаунт создан");
                emit loginSuccess(session);
            },
            [buttons, statusLabel](const QString &error) {
                buttons->setEnabled(true);
                statusLabel->setText(error);
                statusLabel->setStyleSheet("color: #b91c1c; font-size: 13px; font-weight: 700;");
                statusLabel->show();
            });
    });

    dialog.exec();
}
