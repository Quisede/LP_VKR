#include "profilepage.h"

#include <QFormLayout>
#include <QFrame>
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
    pageLayout->addWidget(infoCard);

    rootLayout->addWidget(pageCard);
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
}
