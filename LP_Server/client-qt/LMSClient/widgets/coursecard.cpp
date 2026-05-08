#include "coursecard.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QPushButton>
#include <QVBoxLayout>

CourseCard::CourseCard(
    int courseId,
    const QString &title,
    const QString &description,
    bool showEnrollAction,
    QWidget *parent)
    : QWidget(parent)
    , m_courseId(courseId)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    auto *card = new QFrame(this);
    card->setObjectName("courseCard");
    card->setMinimumHeight(showEnrollAction ? 196 : 206);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 16, 18, 16);
    cardLayout->setSpacing(10);

    auto *badgeRow = new QHBoxLayout();
    badgeRow->setSpacing(8);

    auto *modeBadge = new QLabel(showEnrollAction ? "Student view" : "Teacher view", card);
    modeBadge->setStyleSheet(
        "QLabel {"
        " background: #eef5ff;"
        " color: #2563eb;"
        " border-radius: 10px;"
        " padding: 6px 10px;"
        " font-size: 12px;"
        " font-weight: 700;"
        "}");

    auto *contentBadge = new QLabel(showEnrollAction ? "Материалы и тесты" : "Контент и структура", card);
    contentBadge->setStyleSheet(
        "QLabel {"
        " background: #f1f5f9;"
        " color: #475569;"
        " border-radius: 10px;"
        " padding: 6px 10px;"
        " font-size: 12px;"
        " font-weight: 600;"
        "}");

    badgeRow->addWidget(modeBadge);
    badgeRow->addWidget(contentBadge);
    badgeRow->addStretch();

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *descriptionLabel = new QLabel(
        description.isEmpty() ? "Описание курса пока не добавлено." : description,
        card);
    descriptionLabel->setObjectName("courseCardDescriptionLabel");
    descriptionLabel->setWordWrap(true);

    auto *metaLabel = new QLabel(
        showEnrollAction
            ? "Открой курс, чтобы посмотреть программу, материалы и доступные тесты."
            : "Режим преподавателя: открой курс для обзора или перейди в конструктор для редактирования.",
        card);
    metaLabel->setObjectName("sectionHintLabel");
    metaLabel->setWordWrap(true);

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(10);
    actionsLayout->setContentsMargins(0, 6, 0, 0);

    auto *openButton = new QPushButton("Открыть курс", card);
    QPushButton *enrollButton = nullptr;
    QPushButton *manageButton = nullptr;
    QPushButton *editButton = nullptr;
    QPushButton *deleteButton = nullptr;
    openButton->setObjectName("cardActionButton");
    openButton->setMinimumWidth(150);
    openButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    actionsLayout->addWidget(openButton);
    if (showEnrollAction) {
        enrollButton = new QPushButton("Записаться", card);
        enrollButton->setObjectName("cardAccentButton");
        enrollButton->setMinimumWidth(140);
        enrollButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(enrollButton);
    } else {
        manageButton = new QPushButton("Конструктор", card);
        manageButton->setObjectName("cardAccentButton");
        manageButton->setMinimumWidth(140);
        manageButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(manageButton);

        editButton = new QPushButton("Редактировать", card);
        editButton->setObjectName("cardGhostButton");
        editButton->setMinimumWidth(150);
        editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(editButton);

        deleteButton = new QPushButton("Удалить", card);
        deleteButton->setObjectName("cardDangerButton");
        deleteButton->setMinimumWidth(120);
        deleteButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(deleteButton);
    }
    actionsLayout->addStretch();

    cardLayout->addLayout(badgeRow);
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(descriptionLabel);
    cardLayout->addWidget(metaLabel);
    cardLayout->addLayout(actionsLayout);

    rootLayout->addWidget(card);

    connect(openButton, &QPushButton::clicked, this, [this]() {
        emit openRequested(m_courseId);
    });

    if (enrollButton != nullptr) {
        connect(enrollButton, &QPushButton::clicked, this, [this]() {
            emit enrollRequested(m_courseId);
        });
    }

    if (manageButton != nullptr) {
        connect(manageButton, &QPushButton::clicked, this, [this]() {
            emit builderRequested(m_courseId);
        });
    }

    if (editButton != nullptr) {
        connect(editButton, &QPushButton::clicked, this, [this]() {
            emit editRequested(m_courseId);
        });
    }

    if (deleteButton != nullptr) {
        connect(deleteButton, &QPushButton::clicked, this, [this]() {
            emit deleteRequested(m_courseId);
        });
    }
}
