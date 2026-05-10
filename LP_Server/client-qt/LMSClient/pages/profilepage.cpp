#include "profilepage.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
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
        "Здесь собраны основные данные активной сессии пользователя.",
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

    auto *userIdLabel = new QLabel("User ID", infoCard);
    userIdLabel->setObjectName("profileKeyLabel");
    m_userIdValueLabel = new QLabel("-", infoCard);
    m_userIdValueLabel->setObjectName("profileNameValueLabel");

    auto *tokenLabel = new QLabel("Токен", infoCard);
    tokenLabel->setObjectName("profileKeyLabel");
    m_tokenValueLabel = new QLabel("Пусто", infoCard);
    m_tokenValueLabel->setObjectName("profileTokenValueLabel");
    m_tokenValueLabel->setWordWrap(true);

    formLayout->addRow(nameLabel, m_nameValueLabel);
    formLayout->addRow(roleLabel, m_roleValueLabel);
    formLayout->addRow(userIdLabel, m_userIdValueLabel);
    formLayout->addRow(tokenLabel, m_tokenValueLabel);

    pageLayout->addWidget(titleLabel);
    pageLayout->addWidget(hintLabel);
    pageLayout->addLayout(summaryLayout);
    pageLayout->addWidget(m_introTitleLabel);
    pageLayout->addWidget(m_introTextLabel);
    pageLayout->addWidget(infoCard);

    rootLayout->addWidget(pageCard);

    m_summaryOneTitleLabel->setText("Роль");
    m_summaryOneValueLabel->setText("—");
    m_summaryTwoTitleLabel->setText("Сессия");
    m_summaryTwoValueLabel->setText("Активна");
    m_introTitleLabel->setText("Текущий режим");
    m_introTextLabel->setText("После входа здесь отображаются базовые сведения об активной сессии пользователя.");
}

void ProfilePage::setSession(const SessionData &session)
{
    m_session = session;
    const QString displayName = session.login.isEmpty()
        ? QString("Пользователь #%1").arg(session.userId)
        : session.login;

    m_nameValueLabel->setText(displayName);
    m_roleValueLabel->setText(session.role);
    m_userIdValueLabel->setText(QString::number(session.userId));
    m_tokenValueLabel->setText(session.token.left(24) + (session.token.size() > 24 ? "..." : ""));

    if (session.role == "Teacher") {
        m_summaryOneTitleLabel->setText("Роль");
        m_summaryOneValueLabel->setText("Teacher");
        m_summaryTwoTitleLabel->setText("Режим");
        m_summaryTwoValueLabel->setText("Управление курсами");
        m_introTitleLabel->setText("Кабинет преподавателя");
        m_introTextLabel->setText(
            "Здесь начинается teacher-flow: создание курсов, управление материалами, тестами, студентами и аналитикой.");
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
            "Используй этот кабинет как опорную точку: отсюда видно, под какой ролью открыт доступ и какой пользователь сейчас работает с платформой.");
    }

    refreshLearningSummary();
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
            QString("Сейчас у тебя %1 доступных курсов. Начни с материалов и первого теста — после этого здесь появится учебная динамика.")
                .arg(coursesCount));
    } else {
        m_introTextLabel->setText(
            QString("У тебя уже %1 попыток, из них успешно: %2. Продолжай обучение и улучшай средний результат по тестам.")
                .arg(attemptsCount)
                .arg(passedCount));
    }
}
