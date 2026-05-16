#include "profilepage.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

double averageAttemptScore(const QVector<AttemptData> &attempts)
{
    if (attempts.isEmpty()) {
        return 0.0;
    }

    double total = 0.0;
    for (const auto &attempt : attempts) {
        total += attempt.percentage;
    }
    return total / static_cast<double>(attempts.size());
}

int passedAttemptsCount(const QVector<AttemptData> &attempts)
{
    int count = 0;
    for (const auto &attempt : attempts) {
        if (attempt.passed) {
            ++count;
        }
    }
    return count;
}

}

ProfilePage::ProfilePage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("profileCard");

    auto *pageLayout = new QVBoxLayout(pageCard);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->setSpacing(14);

    auto *titleLabel = new QLabel("Профиль", pageCard);
    titleLabel->setObjectName("profileSectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Здесь собраны контактные данные, учебная информация и настройки безопасности.",
        pageCard);
    hintLabel->setObjectName("profileHintLabel");
    hintLabel->setWordWrap(true);

    auto createStatCard = [pageCard](QLabel **titleLabelOut, QLabel **valueLabelOut) {
        auto *card = new QFrame(pageCard);
        card->setObjectName("moduleCard");
        card->setMinimumHeight(136);
        card->setMaximumHeight(156);

        auto *layout = new QVBoxLayout(card);
        layout->setContentsMargins(18, 18, 18, 18);
        layout->setSpacing(8);

        *titleLabelOut = new QLabel(card);
        (*titleLabelOut)->setObjectName("moduleTitleLabel");

        *valueLabelOut = new QLabel(card);
        (*valueLabelOut)->setObjectName("courseDetailTitleLabel");

        layout->addWidget(*titleLabelOut);
        layout->addWidget(*valueLabelOut);
        layout->addStretch();
        return card;
    };

    auto *summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(14);
    summaryLayout->addWidget(createStatCard(&m_summaryOneTitleLabel, &m_summaryOneValueLabel));
    summaryLayout->addWidget(createStatCard(&m_summaryTwoTitleLabel, &m_summaryTwoValueLabel));

    m_introTitleLabel = new QLabel(pageCard);
    m_introTitleLabel->setObjectName("moduleTitleLabel");
    m_introTextLabel = new QLabel(pageCard);
    m_introTextLabel->setObjectName("sectionHintLabel");
    m_introTextLabel->setWordWrap(true);

    auto *infoCard = new QFrame(pageCard);
    infoCard->setObjectName("profileInfoCard");

    auto *formLayout = new QFormLayout(infoCard);
    formLayout->setHorizontalSpacing(18);
    formLayout->setVerticalSpacing(14);

    auto *nameLabel = new QLabel("Пользователь", infoCard);
    nameLabel->setObjectName("profileKeyLabel");
    m_nameValueLabel = new QLabel("Неизвестно", infoCard);
    m_nameValueLabel->setObjectName("profileNameValueLabel");

    auto *roleLabel = new QLabel("Роль", infoCard);
    roleLabel->setObjectName("profileKeyLabel");
    m_roleValueLabel = new QLabel("Неизвестно", infoCard);
    m_roleValueLabel->setObjectName("profileRoleValueLabel");

    auto *loginLabel = new QLabel("Логин", infoCard);
    loginLabel->setObjectName("profileKeyLabel");
    m_loginValueLabel = new QLabel("-", infoCard);
    m_loginValueLabel->setObjectName("profileNameValueLabel");

    auto *groupLabel = new QLabel("Группа / подразделение", infoCard);
    groupLabel->setObjectName("profileKeyLabel");
    m_groupValueLabel = new QLabel("-", infoCard);
    m_groupValueLabel->setObjectName("profileNameValueLabel");

    auto *emailLabel = new QLabel("Почта", infoCard);
    emailLabel->setObjectName("profileKeyLabel");
    m_emailValueLabel = new QLabel("-", infoCard);
    m_emailValueLabel->setObjectName("profileNameValueLabel");

    auto *phoneLabel = new QLabel("Телефон", infoCard);
    phoneLabel->setObjectName("profileKeyLabel");
    m_phoneValueLabel = new QLabel("-", infoCard);
    m_phoneValueLabel->setObjectName("profileNameValueLabel");

    formLayout->addRow(nameLabel, m_nameValueLabel);
    formLayout->addRow(roleLabel, m_roleValueLabel);
    formLayout->addRow(loginLabel, m_loginValueLabel);
    formLayout->addRow(groupLabel, m_groupValueLabel);
    formLayout->addRow(emailLabel, m_emailValueLabel);
    formLayout->addRow(phoneLabel, m_phoneValueLabel);

    auto *passwordCard = new QFrame(pageCard);
    passwordCard->setObjectName("profileInfoCard");
    auto *passwordLayout = new QVBoxLayout(passwordCard);
    passwordLayout->setContentsMargins(18, 18, 18, 18);
    passwordLayout->setSpacing(12);

    auto *passwordTitleLabel = new QLabel("Смена пароля", passwordCard);
    passwordTitleLabel->setObjectName("moduleTitleLabel");

    auto *passwordHintLabel = new QLabel(
        "Для безопасности нужно указать текущий пароль, затем новый пароль и повтор.",
        passwordCard);
    passwordHintLabel->setObjectName("sectionHintLabel");
    passwordHintLabel->setWordWrap(true);

    auto *passwordForm = new QFormLayout();
    passwordForm->setHorizontalSpacing(18);
    passwordForm->setVerticalSpacing(10);

    m_oldPasswordEdit = new QLineEdit(passwordCard);
    m_oldPasswordEdit->setPlaceholderText("Текущий пароль");
    m_oldPasswordEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordEdit = new QLineEdit(passwordCard);
    m_newPasswordEdit->setPlaceholderText("Новый пароль");
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_repeatPasswordEdit = new QLineEdit(passwordCard);
    m_repeatPasswordEdit->setPlaceholderText("Повторите новый пароль");
    m_repeatPasswordEdit->setEchoMode(QLineEdit::Password);

    const QString inputStyle =
        "QLineEdit { background: #ffffff; border: 1px solid #dbe4f0; border-radius: 12px; padding: 10px 12px; color: #0f172a; }"
        "QLineEdit:focus { border-color: #2563eb; }";
    m_oldPasswordEdit->setStyleSheet(inputStyle);
    m_newPasswordEdit->setStyleSheet(inputStyle);
    m_repeatPasswordEdit->setStyleSheet(inputStyle);

    passwordForm->addRow("Текущий пароль", m_oldPasswordEdit);
    passwordForm->addRow("Новый пароль", m_newPasswordEdit);
    passwordForm->addRow("Повтор", m_repeatPasswordEdit);

    auto *passwordActions = new QHBoxLayout();
    passwordActions->setSpacing(10);
    m_changePasswordButton = new QPushButton("Сменить пароль", passwordCard);
    m_changePasswordButton->setObjectName("cardAccentButton");
    m_passwordStatusLabel = new QLabel(passwordCard);
    m_passwordStatusLabel->setObjectName("sectionHintLabel");
    m_passwordStatusLabel->setWordWrap(true);
    m_passwordStatusLabel->hide();

    passwordActions->addWidget(m_changePasswordButton);
    passwordActions->addWidget(m_passwordStatusLabel, 1);

    passwordLayout->addWidget(passwordTitleLabel);
    passwordLayout->addWidget(passwordHintLabel);
    passwordLayout->addLayout(passwordForm);
    passwordLayout->addLayout(passwordActions);

    pageLayout->addWidget(titleLabel);
    pageLayout->addWidget(hintLabel);
    pageLayout->addLayout(summaryLayout);
    pageLayout->addWidget(m_introTitleLabel);
    pageLayout->addWidget(m_introTextLabel);
    pageLayout->addWidget(infoCard);
    pageLayout->addWidget(passwordCard);

    rootLayout->addWidget(pageCard);

    m_summaryOneTitleLabel->setText("Роль");
    m_summaryOneValueLabel->setText("—");
    m_summaryTwoTitleLabel->setText("Сессия");
    m_summaryTwoValueLabel->setText("Активна");
    m_introTitleLabel->setText("Текущий режим");
    m_introTextLabel->setText("После входа здесь отображаются базовые сведения об активной сессии пользователя.");

    connect(m_changePasswordButton, &QPushButton::clicked, this, [this]() {
        const QString oldPassword = m_oldPasswordEdit->text();
        const QString newPassword = m_newPasswordEdit->text();
        const QString repeatPassword = m_repeatPasswordEdit->text();

        if (oldPassword.isEmpty() || newPassword.isEmpty() || repeatPassword.isEmpty()) {
            showPasswordMessage("Заполните все поля для смены пароля.", true);
            return;
        }

        if (newPassword != repeatPassword) {
            showPasswordMessage("Новый пароль и повтор не совпадают.", true);
            return;
        }

        if (newPassword.size() < 6) {
            showPasswordMessage("Новый пароль должен содержать минимум 6 символов.", true);
            return;
        }

        emit changePasswordRequested(oldPassword, newPassword);
    });
}

void ProfilePage::setSession(const SessionData &session)
{
    m_session = session;
    const QString displayName = session.fullName.trimmed().isEmpty()
        ? (session.login.isEmpty() ? "Пользователь" : session.login)
        : session.fullName.trimmed();

    m_nameValueLabel->setText(displayName);
    m_roleValueLabel->setText(session.role);
    m_loginValueLabel->setText(session.login.isEmpty() ? "-" : session.login);
    m_groupValueLabel->setText(session.groupName.isEmpty() ? "-" : session.groupName);
    m_emailValueLabel->setText(session.email.isEmpty() ? "-" : session.email);
    m_phoneValueLabel->setText(session.phone.isEmpty() ? "-" : session.phone);

    if (session.role == "Teacher") {
        m_summaryOneTitleLabel->setText("Роль");
        m_summaryOneValueLabel->setText("Teacher");
        m_summaryTwoTitleLabel->setText("Режим");
        m_summaryTwoValueLabel->setText("Управление курсами");
        m_introTitleLabel->setText("Кабинет преподавателя");
        m_introTextLabel->setText(
            "Здесь начинается рабочее пространство преподавателя: создание курсов, управление материалами, тестами, студентами и аналитикой.");
    } else if (session.role == "Admin") {
        m_summaryOneTitleLabel->setText("Роль");
        m_summaryOneValueLabel->setText("Admin");
        m_summaryTwoTitleLabel->setText("Режим");
        m_summaryTwoValueLabel->setText("Системный доступ");
        m_introTitleLabel->setText("Панель администратора");
        m_introTextLabel->setText(
            "Текущая сессия открыта в административном режиме. Здесь будут собраны системные инструменты и общая статистика.");
    } else {
        m_summaryOneTitleLabel->setText("Курсы");
        m_summaryTwoTitleLabel->setText("Средний балл");
        m_introTitleLabel->setText("Учебный профиль");
        m_introTextLabel->setText(
            "Кабинет показывает учебный статус, контактные данные и параметры безопасности активной учётной записи.");
    }

    refreshLearningSummary();
}

void ProfilePage::showPasswordMessage(const QString &message, bool isError)
{
    m_passwordStatusLabel->setText(message);
    m_passwordStatusLabel->setStyleSheet(QString("color: %1; font-size: 13px; font-weight: 700;")
        .arg(isError ? "#b91c1c" : "#047857"));
    m_passwordStatusLabel->show();
}

void ProfilePage::setPasswordBusy(bool busy)
{
    m_oldPasswordEdit->setEnabled(!busy);
    m_newPasswordEdit->setEnabled(!busy);
    m_repeatPasswordEdit->setEnabled(!busy);
    m_changePasswordButton->setEnabled(!busy);

    if (busy) {
        showPasswordMessage("Обновляем пароль...", false);
        return;
    }

    m_oldPasswordEdit->clear();
    m_newPasswordEdit->clear();
    m_repeatPasswordEdit->clear();
}

void ProfilePage::setCourses(const QVector<CourseData> &courses)
{
    m_courses = courses;
    refreshLearningSummary();
}

void ProfilePage::setAttempts(const QVector<AttemptData> &attempts)
{
    m_attempts = attempts;
    refreshLearningSummary();
}

void ProfilePage::refreshLearningSummary()
{
    if (m_session.role == "Teacher") {
        m_summaryOneTitleLabel->setText("Роль");
        m_summaryOneValueLabel->setText("Teacher");
        m_summaryTwoTitleLabel->setText("Режим");
        m_summaryTwoValueLabel->setText("Управление курсами");
        return;
    }

    if (m_session.role == "Admin") {
        m_summaryOneTitleLabel->setText("Роль");
        m_summaryOneValueLabel->setText("Admin");
        m_summaryTwoTitleLabel->setText("Режим");
        m_summaryTwoValueLabel->setText("Системный доступ");
        return;
    }

    const int coursesCount = m_courses.size();
    const int attemptsCount = m_attempts.size();
    const int passedCount = passedAttemptsCount(m_attempts);
    const double average = averageAttemptScore(m_attempts);

    m_summaryOneTitleLabel->setText("Курсы");
    m_summaryOneValueLabel->setText(QString::number(coursesCount));
    m_summaryTwoTitleLabel->setText("Средний балл");
    m_summaryTwoValueLabel->setText(QString("%1%").arg(QString::number(average, 'f', 1)));

    if (attemptsCount == 0) {
        m_introTextLabel->setText(
            QString("Сейчас доступно %1 курсов. После изучения материалов и первого теста здесь появится учебная динамика.")
                .arg(coursesCount));
    } else {
        m_introTextLabel->setText(
            QString("Всего попыток: %1, успешных: %2. Продолжение обучения улучшит средний результат по тестам.")
                .arg(attemptsCount)
                .arg(passedCount));
    }
}
