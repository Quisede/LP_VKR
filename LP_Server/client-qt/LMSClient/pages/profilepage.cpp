#include "profilepage.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

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
        m_summaryOneTitleLabel->setText("Роль");
        m_summaryOneValueLabel->setText("Student");
        m_summaryTwoTitleLabel->setText("Режим");
        m_summaryTwoValueLabel->setText("Обучение");
        m_introTitleLabel->setText("Учебный профиль");
        m_introTextLabel->setText(
            "Используй этот кабинет как опорную точку: отсюда видно, под какой ролью открыт доступ и какой пользователь сейчас работает с платформой.");
    }
}
