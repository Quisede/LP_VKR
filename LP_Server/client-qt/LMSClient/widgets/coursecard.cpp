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

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 16, 18, 16);
    cardLayout->setSpacing(10);

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
            ? "Открой курс, чтобы посмотреть материалы и тесты."
            : "Режим преподавателя: курс можно открыть или перейти в конструктор.",
        card);
    metaLabel->setObjectName("sectionHintLabel");
    metaLabel->setWordWrap(true);

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(10);
    actionsLayout->setContentsMargins(0, 6, 0, 0);

    auto *openButton = new QPushButton("Открыть курс", card);
    QPushButton *enrollButton = nullptr;
    QPushButton *manageButton = nullptr;
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
    }
    actionsLayout->addStretch();

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
}
