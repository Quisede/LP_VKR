#include "coursecard.h"

#include "../models/coursemodel.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QSizePolicy>
#include <QPushButton>
#include <QVBoxLayout>

CourseCard::CourseCard(
    const CourseData &course,
    const QString &role,
    QWidget *parent)
    : QWidget(parent)
    , m_courseId(course.id)
{
    const bool isStudent = role == "Student";
    const bool isAdmin = role == "Admin";

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    auto *card = new QFrame(this);
    card->setObjectName("courseCard");
    card->setMinimumHeight(isStudent ? 236 : 246);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 16, 18, 16);
    cardLayout->setSpacing(12);

    auto *badgeRow = new QHBoxLayout();
    badgeRow->setSpacing(8);

    auto *modeBadge = new QLabel(isStudent ? "Student view" : isAdmin ? "Admin view" : "Teacher view", card);
    modeBadge->setStyleSheet(
        "QLabel {"
        " background: #eef5ff;"
        " color: #2563eb;"
        " border-radius: 10px;"
        " padding: 6px 10px;"
        " font-size: 12px;"
        " font-weight: 700;"
        "}");

    auto *contentBadge = new QLabel(isStudent ? "Материалы и тесты" : isAdmin ? "Система и контроль" : "Контент и структура", card);
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

    auto *titleLabel = new QLabel(course.title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *descriptionLabel = new QLabel(
        course.description.isEmpty() ? "Описание курса пока не добавлено." : course.description,
        card);
    descriptionLabel->setObjectName("courseCardDescriptionLabel");
    descriptionLabel->setWordWrap(true);

    auto *statsRow = new QHBoxLayout();
    statsRow->setSpacing(8);

    auto createStatBadge = [card](const QString &text, const QString &bg, const QString &fg) {
        auto *badge = new QLabel(text, card);
        badge->setStyleSheet(QString(
            "QLabel {"
            " background: %1;"
            " color: %2;"
            " border-radius: 10px;"
            " padding: 6px 10px;"
            " font-size: 12px;"
            " font-weight: 700;"
            "}").arg(bg, fg));
        return badge;
    };

    statsRow->addWidget(createStatBadge(
        QString("Уроков %1").arg(course.lessonsCount),
        "#f8fafc",
        "#334155"));
    statsRow->addWidget(createStatBadge(
        QString("Тестов %1").arg(course.testsCount),
        "#eff6ff",
        "#2563eb"));
    const QString teacherName = course.teacherName.trimmed().isEmpty()
        ? QString("Преподаватель")
        : course.teacherName.trimmed();

    statsRow->addWidget(createStatBadge(
        isStudent
            ? QString("Преподаватель: %1").arg(teacherName)
            : QString("Студентов %1").arg(course.studentsCount),
        isStudent ? "#f8fafc" : "#ecfeff",
        isStudent ? "#475569" : "#0f766e"));
    statsRow->addStretch();

    auto *metaLabel = new QLabel(
        isStudent
            ? (course.testsCount > 0
                ? QString("Прогресс по тестам: %1 из %2 зачтено. Открытие курса покажет следующий шаг.")
                    .arg(course.passedTestsCount)
                    .arg(course.testsCount)
                : "В курсе пока нет тестов. Открытие курса покажет уроки и материалы.")
            : isAdmin
                ? "Административный режим: проверка структуры курса и переход к управлению, если нужна системная правка."
                : "Режим преподавателя: обзор курса или переход в конструктор для управления контентом и тестами.",
        card);
    metaLabel->setObjectName("sectionHintLabel");
    metaLabel->setWordWrap(true);

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(8);
    actionsLayout->setContentsMargins(0, 6, 0, 0);

    auto *openButton = new QPushButton("Открыть курс", card);
    QPushButton *enrollButton = nullptr;
    QPushButton *manageButton = nullptr;
    QPushButton *editButton = nullptr;
    QPushButton *deleteButton = nullptr;
    openButton->setObjectName("cardActionButton");
    openButton->setMinimumWidth(isStudent ? 150 : 132);
    openButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    actionsLayout->addWidget(openButton);
    if (isStudent) {
        enrollButton = new QPushButton("Записаться", card);
        enrollButton->setObjectName("cardAccentButton");
        enrollButton->setMinimumWidth(140);
        enrollButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(enrollButton);
    } else {
        manageButton = new QPushButton("Конструктор", card);
        manageButton->setObjectName("cardAccentButton");
        manageButton->setMinimumWidth(128);
        manageButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(manageButton);

        editButton = new QPushButton("Редактировать", card);
        editButton->setObjectName("cardGhostButton");
        editButton->setMinimumWidth(128);
        editButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(editButton);

        deleteButton = new QPushButton("Удалить", card);
        deleteButton->setObjectName("cardDangerButton");
        deleteButton->setMinimumWidth(108);
        deleteButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        actionsLayout->addWidget(deleteButton);
    }
    actionsLayout->addStretch();

    cardLayout->addLayout(badgeRow);
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(descriptionLabel);
    cardLayout->addLayout(statsRow);
    if (isStudent) {
        auto *progressBar = new QProgressBar(card);
        progressBar->setRange(0, 100);
        progressBar->setValue(course.progressPercent);
        progressBar->setTextVisible(true);
        progressBar->setFormat(QString("Прогресс: %1%").arg(course.progressPercent));
        progressBar->setFixedHeight(16);
        progressBar->setStyleSheet(
            "QProgressBar {"
            " background: #e2e8f0;"
            " border: none;"
            " border-radius: 8px;"
            " color: #0f172a;"
            " font-size: 11px;"
            " font-weight: 700;"
            " text-align: center;"
            "}"
            "QProgressBar::chunk {"
            " background: #2563eb;"
            " border-radius: 8px;"
            "}");
        cardLayout->addWidget(progressBar);
    }
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
