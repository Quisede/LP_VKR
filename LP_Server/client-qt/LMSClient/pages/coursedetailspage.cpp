#include "coursedetailspage.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSizePolicy>
#include <QTabWidget>
#include <QVBoxLayout>
#include <algorithm>
#include <functional>

namespace {
QString normalizedReaderText(QString text)
{
    text.replace("\\r\\n", "\n");
    text.replace("\\n", "\n");
    text.replace("\\t", "    ");
    return text.trimmed();
}

QString compactPreview(const QString &rawText, int limit = 180)
{
    QString text = normalizedReaderText(rawText);
    text.replace('\n', " ");
    text = text.simplified();
    if (text.size() <= limit) {
        return text;
    }
    return text.left(limit).trimmed() + "...";
}

bool parseEmbeddedFileMaterial(const QString &content, QJsonObject *payload)
{
    const QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (!doc.isObject()) {
        return false;
    }

    const QJsonObject object = doc.object();
    if (object.value("kind").toString() != "embedded-file") {
        return false;
    }

    if (payload != nullptr) {
        *payload = object;
    }
    return true;
}

QString materialSubtitle(const MaterialData &material)
{
    QJsonObject payload;
    if (parseEmbeddedFileMaterial(material.content, &payload)) {
        const QString fileName = payload.value("fileName").toString(material.title);
        const QString mimeType = payload.value("mimeType").toString("application/octet-stream");
        const int size = payload.value("size").toInt(0);
        return QString("Файл: %1\nТип: %2  •  MIME: %3  •  Размер: %4 КБ")
            .arg(fileName)
            .arg(material.type)
            .arg(mimeType)
            .arg((size + 1023) / 1024);
    }

    return compactPreview(material.content);
}

bool isLinkLikeMaterial(const MaterialData &material)
{
    return material.type == "link" || material.type == "video";
}

bool isTextLikeMaterial(const MaterialData &material)
{
    return material.type == "text" && !parseEmbeddedFileMaterial(material.content, nullptr);
}

void appendCard(QListWidget *list, const QString &title, const QString &subtitle, const QVariant &payload = {})
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 88));
    if (payload.isValid()) {
        item->setData(Qt::UserRole, payload);
    }

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(6);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *subtitleLabel = new QLabel(subtitle, card);
    subtitleLabel->setObjectName("courseCardDescriptionLabel");
    subtitleLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    list->addItem(item);
    list->setItemWidget(item, card);
}

void appendMaterialCard(QListWidget *list, const MaterialData &material, CourseDetailsPage *context)
{
    QJsonObject payload;
    const bool isEmbeddedFile = parseEmbeddedFileMaterial(material.content, &payload);
    const bool isLinkLike = isLinkLikeMaterial(material);
    const bool isTextLike = isTextLikeMaterial(material);

    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, (isEmbeddedFile || isLinkLike || isTextLike) ? 142 : 96));
    item->setData(Qt::UserRole, material.id);

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(8);

    const QString title = isEmbeddedFile
        ? QString("%1 (%2)").arg(material.title, material.type.toUpper())
        : material.title;

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *subtitleLabel = new QLabel(materialSubtitle(material), card);
    subtitleLabel->setObjectName("courseCardDescriptionLabel");
    subtitleLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    if (isEmbeddedFile || isLinkLike || isTextLike) {
        auto *actionsLayout = new QHBoxLayout();
        actionsLayout->setSpacing(10);

        if (isTextLike) {
            auto *previewButton = new QPushButton("Открыть материал", card);
            previewButton->setObjectName("cardGhostButton");
            actionsLayout->addWidget(previewButton);

            QObject::connect(previewButton, &QPushButton::clicked, context, [context, material]() {
                emit context->materialReaderRequested(material.id);
            });
        }

        if (isLinkLike) {
            auto *openLinkButton = new QPushButton(material.type == "video" ? "Открыть видео" : "Открыть ссылку", card);
            openLinkButton->setObjectName("cardGhostButton");
            actionsLayout->addWidget(openLinkButton);

            QObject::connect(openLinkButton, &QPushButton::clicked, context, [context, material]() {
                emit context->materialLinkOpenRequested(material.content);
            });
        }

        if (isEmbeddedFile) {
            auto *saveButton = new QPushButton("Скачать файл", card);
            saveButton->setObjectName("cardGhostButton");
            actionsLayout->addWidget(saveButton);

            auto *openExternalButton = new QPushButton("Открыть во внешнем приложении", card);
            openExternalButton->setObjectName("cardAccentButton");
            actionsLayout->addWidget(openExternalButton);

            QObject::connect(saveButton, &QPushButton::clicked, context, [context, material]() {
                emit context->materialDownloadRequested(material.id);
            });
            QObject::connect(openExternalButton, &QPushButton::clicked, context, [context, material]() {
                emit context->materialOpenExternalRequested(material.id);
            });
        }

        actionsLayout->addStretch();
        layout->addLayout(actionsLayout);
    }

    list->addItem(item);
    list->setItemWidget(item, card);
}

void appendLessonCard(QListWidget *list, const LessonData &lesson, CourseDetailsPage *context, bool studentMode)
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, studentMode ? 142 : 132));
    item->setData(Qt::UserRole, lesson.id);

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(lesson.completed ? QString("%1  •  изучено").arg(lesson.title) : lesson.title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *subtitleLabel = new QLabel(compactPreview(lesson.content), card);
    subtitleLabel->setObjectName("courseCardDescriptionLabel");
    subtitleLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(10);

    auto *previewButton = new QPushButton("Открыть урок", card);
    previewButton->setObjectName("cardGhostButton");
    actionsLayout->addWidget(previewButton);

    QObject::connect(previewButton, &QPushButton::clicked, context, [context, lesson]() {
        emit context->lessonReaderRequested(lesson.id);
    });

    if (studentMode) {
        auto *completeButton = new QPushButton(lesson.completed ? "Изучено" : "Отметить изученным", card);
        completeButton->setObjectName(lesson.completed ? "cardGhostButton" : "cardAccentButton");
        completeButton->setEnabled(!lesson.completed);
        actionsLayout->addWidget(completeButton);

        QObject::connect(completeButton, &QPushButton::clicked, context, [context, lesson]() {
            emit context->lessonCompletedRequested(lesson.id);
        });
    }
    actionsLayout->addStretch();
    layout->addLayout(actionsLayout);

    list->addItem(item);
    list->setItemWidget(item, card);
}

void appendTestCard(
    QListWidget *list,
    int testId,
    const QString &title,
    const QString &subtitle,
    QObject *context,
    const std::function<void()> &onStart,
    bool enabled = true)
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 150));
    item->setData(Qt::UserRole, testId);
    item->setData(Qt::UserRole + 1, title);

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");
    card->setMinimumHeight(136);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(9);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *subtitleLabel = new QLabel(subtitle, card);
    subtitleLabel->setObjectName("courseCardDescriptionLabel");
    subtitleLabel->setWordWrap(true);

    auto *startButton = new QPushButton("Перейти к тесту", card);
    startButton->setObjectName("testOpenButton");
    startButton->setEnabled(enabled);
    startButton->setMinimumHeight(44);
    startButton->setMinimumWidth(178);
    startButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    startButton->setCursor(enabled ? Qt::PointingHandCursor : Qt::ArrowCursor);
    startButton->setStyleSheet(
        enabled
            ? "QPushButton#testOpenButton {"
              " background-color: #2563eb;"
              " color: #ffffff;"
              " border: 1px solid #1d4ed8;"
              " border-radius: 14px;"
              " padding: 0 18px;"
              " font-size: 14px;"
              " font-weight: 700;"
              "}"
              "QPushButton#testOpenButton:hover { background-color: #1d4ed8; }"
            : "QPushButton#testOpenButton {"
              " background-color: #e2e8f0;"
              " color: #64748b;"
              " border: 1px solid #cbd5e1;"
              " border-radius: 14px;"
              " padding: 0 18px;"
              " font-size: 14px;"
              " font-weight: 700;"
              "}");

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setSpacing(10);
    actionsLayout->addWidget(startButton, 0, Qt::AlignLeft);
    actionsLayout->addStretch();

    layout->addWidget(titleLabel);
    layout->addLayout(actionsLayout);
    layout->addWidget(subtitleLabel);

    if (enabled) {
        QObject::connect(startButton, &QPushButton::clicked, context, onStart);
    }

    list->addItem(item);
    list->setItemWidget(item, card);
}
}

CourseDetailsPage::CourseDetailsPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("courseDetailCard");

    auto *pageLayout = new QVBoxLayout(pageCard);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->setSpacing(14);

    auto *topLayout = new QHBoxLayout();
    auto *backButton = new QPushButton("Назад к курсам", pageCard);
    backButton->setObjectName("backToCoursesButton");
    m_primaryActionButton = new QPushButton("Записаться на курс", pageCard);
    m_primaryActionButton->setObjectName("enrollButton");
    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(m_primaryActionButton);

    m_titleLabel = new QLabel("Курс не выбран", pageCard);
    m_titleLabel->setObjectName("courseDetailTitleLabel");
    m_titleLabel->setWordWrap(true);

    m_descriptionLabel = new QLabel(
        "Сначала нужно открыть раздел курсов и нажать на карточку нужного курса.",
        pageCard);
    m_descriptionLabel->setObjectName("courseDetailDescriptionLabel");
    m_descriptionLabel->setWordWrap(true);

    auto *summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(14);

    auto createSummaryCard = [pageCard](const QString &title, QLabel **valueLabel) {
        auto *card = new QFrame(pageCard);
        card->setObjectName("moduleCard");
        card->setMinimumHeight(136);
        card->setMaximumHeight(156);
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 18, 18, 18);
        cardLayout->setSpacing(8);

        auto *titleLabel = new QLabel(title, card);
        titleLabel->setObjectName("moduleTitleLabel");

        *valueLabel = new QLabel("0", card);
        (*valueLabel)->setObjectName("courseDetailTitleLabel");

        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(*valueLabel);
        cardLayout->addStretch();
        return card;
    };

    summaryLayout->addWidget(createSummaryCard("Уроки", &m_lessonsSummaryLabel));
    summaryLayout->addWidget(createSummaryCard("Материалы", &m_materialsSummaryLabel));
    summaryLayout->addWidget(createSummaryCard("Видео", &m_videosSummaryLabel));
    summaryLayout->addWidget(createSummaryCard("Тесты", &m_testsSummaryLabel));

    m_overviewHintLabel = new QLabel(pageCard);
    m_overviewHintLabel->setObjectName("sectionHintLabel");
    m_overviewHintLabel->setWordWrap(true);

    m_progressHintLabel = new QLabel(pageCard);
    m_progressHintLabel->setWordWrap(true);
    m_progressHintLabel->setObjectName("sectionHintLabel");
    m_progressHintLabel->setStyleSheet(
        "QLabel {"
        " background: #eff6ff;"
        " color: #1e3a8a;"
        " border: 1px solid #bfdbfe;"
        " border-radius: 16px;"
        " padding: 14px 16px;"
        "}");

    m_sectionsTabs = new QTabWidget(pageCard);
    m_sectionsTabs->setDocumentMode(true);
    m_sectionsTabs->tabBar()->setDrawBase(false);
    m_sectionsTabs->tabBar()->setExpanding(false);
    m_sectionsTabs->setStyleSheet(
        "QTabWidget::pane {"
        " border: 1px solid #dbe4f0;"
        " border-radius: 18px;"
        " margin-top: 12px;"
        " background: #f8fbff;"
        " padding: 18px;"
        "}"
        "QTabWidget::tab-bar { alignment: left; left: 10px; }"
        "QTabBar::tab {"
        " background: #e2e8f0;"
        " color: #334155;"
        " border: none;"
        " border-top-left-radius: 12px;"
        " border-top-right-radius: 12px;"
        " padding: 10px 18px;"
        " min-width: 96px;"
        " font-weight: 600;"
        " margin-right: 8px;"
        "}"
        "QTabBar::tab:selected {"
        " background: #2563eb;"
        " color: #ffffff;"
        "}");

    auto createListTab = [this](const QString &title, QListWidget **listRef) {
        auto *tab = new QWidget(m_sectionsTabs);
        auto *tabLayout = new QVBoxLayout(tab);
        tabLayout->setContentsMargins(0, 0, 0, 0);
        tabLayout->setSpacing(12);

        auto *card = new QFrame(tab);
        card->setObjectName("moduleCard");
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 18, 18, 18);
        cardLayout->setSpacing(12);

        auto *titleLabel = new QLabel(title, card);
        titleLabel->setObjectName("moduleTitleLabel");

        *listRef = new QListWidget(card);
        (*listRef)->setSpacing(10);
        (*listRef)->setSelectionMode(QAbstractItemView::NoSelection);
        (*listRef)->setFrameShape(QFrame::NoFrame);
        (*listRef)->setAttribute(Qt::WA_StyledBackground, true);
        (*listRef)->viewport()->setAttribute(Qt::WA_StyledBackground, true);
        (*listRef)->setStyleSheet(
            "QListWidget { background: transparent; border: none; outline: none; }"
            "QListWidget::item { background: transparent; border: none; margin: 0; padding: 0; }"
            "QListWidget::item:hover, QListWidget::item:selected { background: transparent; }");
        (*listRef)->viewport()->setStyleSheet("background: transparent;");

        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(*listRef);
        tabLayout->addWidget(card);
        return tab;
    };

    auto *overviewTab = new QWidget(m_sectionsTabs);
    auto *overviewLayout = new QVBoxLayout(overviewTab);
    overviewLayout->setContentsMargins(0, 0, 0, 0);
    overviewLayout->setSpacing(14);
    overviewLayout->addLayout(summaryLayout);
    overviewLayout->addWidget(m_overviewHintLabel);
    overviewLayout->addWidget(m_progressHintLabel);
    overviewLayout->addStretch();

    auto *lessonsTab = createListTab("Уроки курса", &m_lessonsList);
    auto *materialsTab = createListTab("Материалы курса", &m_materialsList);
    auto *videosTab = createListTab("Видео и ссылки", &m_videosList);
    auto *testsTab = createListTab("Тесты курса", &m_testsList);
    m_studentsTab = createListTab("Ученики курса", &m_studentsList);

    m_sectionsTabs->addTab(overviewTab, "Обзор");
    m_sectionsTabs->addTab(lessonsTab, "Уроки");
    m_sectionsTabs->addTab(materialsTab, "Материалы");
    m_sectionsTabs->addTab(videosTab, "Видео");
    m_sectionsTabs->addTab(testsTab, "Тесты");
    m_sectionsTabs->addTab(m_studentsTab, "Ученики");

    pageLayout->addLayout(topLayout);
    pageLayout->addWidget(m_titleLabel);
    pageLayout->addWidget(m_descriptionLabel);
    pageLayout->addWidget(m_sectionsTabs);

    rootLayout->addWidget(pageCard);

    connect(backButton, &QPushButton::clicked, this, &CourseDetailsPage::backRequested);
    connect(m_primaryActionButton, &QPushButton::clicked, this, &CourseDetailsPage::enrollRequested);

    refreshOverview();
}

void CourseDetailsPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher" || role == "Admin") {
        m_primaryActionButton->hide();
        if (m_sectionsTabs->indexOf(m_studentsTab) < 0) {
            m_sectionsTabs->addTab(m_studentsTab, "Ученики");
        }
    } else {
        m_primaryActionButton->show();
        const int studentsIndex = m_sectionsTabs->indexOf(m_studentsTab);
        if (studentsIndex >= 0) {
            m_sectionsTabs->removeTab(studentsIndex);
        }
    }
}

void CourseDetailsPage::setCourse(const CourseData &course)
{
    m_course = course;
    if (course.id < 0) {
        m_titleLabel->setText((m_role == "Teacher" || m_role == "Admin") ? "Курс не выбран" : "Курс без названия");
        m_descriptionLabel->setText(
            (m_role == "Teacher" || m_role == "Admin")
                ? "Сначала нужно выбрать курс во вкладке \"Мои курсы\", чтобы открыть конструктор курса."
                : "Для этого курса пока нет подробного описания.");
        refreshOverview();
        return;
    }

    m_titleLabel->setText(course.title.isEmpty() ? "Курс без названия" : course.title);
    m_descriptionLabel->setText(
        course.description.isEmpty()
            ? "Для этого курса пока нет подробного описания."
            : course.description);
    if (m_role == "Student") {
        m_primaryActionButton->setText(course.enrolled ? "Вы уже записаны" : "Записаться на курс");
        m_primaryActionButton->setEnabled(!course.enrolled);
        m_primaryActionButton->setObjectName(course.enrolled ? "cardGhostButton" : "enrollButton");
    }
    refreshOverview();
}

void CourseDetailsPage::setLessons(const QVector<LessonData> &lessons)
{
    m_lessons = lessons;
    m_lessonsList->clear();
    if (lessons.isEmpty()) {
        appendCard(m_lessonsList, "Уроков пока нет", "Для курса ещё не добавлены уроки.");
    } else {
        const bool studentMode = m_role == "Student";
        for (const auto &lesson : lessons) {
            appendLessonCard(m_lessonsList, lesson, this, studentMode);
        }
    }
    m_lessonsSummaryLabel->setText(QString::number(lessons.size()));
    refreshOverview();
}

void CourseDetailsPage::setMaterials(const QVector<MaterialData> &materials, const QVector<MaterialData> &videos)
{
    m_materials = materials;
    m_videos = videos;

    m_materialsList->clear();
    if (materials.isEmpty()) {
        appendCard(m_materialsList, "Материалов пока нет", "У этого курса нет обычных материалов.");
    } else {
        for (const auto &material : materials) {
            appendMaterialCard(m_materialsList, material, this);
        }
    }

    m_videosList->clear();
    if (videos.isEmpty()) {
        appendCard(m_videosList, "Видео пока нет", "У этого курса нет видеоматериалов.");
    } else {
        for (const auto &video : videos) {
            appendMaterialCard(m_videosList, video, this);
        }
    }
    m_materialsSummaryLabel->setText(QString::number(materials.size()));
    m_videosSummaryLabel->setText(QString::number(videos.size()));
    refreshOverview();
}

void CourseDetailsPage::setTests(const QVector<TestData> &tests)
{
    m_tests = tests;
    m_testsList->clear();
    if (tests.isEmpty()) {
        appendCard(m_testsList, "Тестов пока нет", "Для этого курса ещё не добавили тесты.");
        m_testsSummaryLabel->setText("0");
        refreshOverview();
        return;
    }

    for (const auto &test : tests) {
        const QString statusText = test.status == "closed"
            ? "закрыт"
            : test.available ? "активен" : "дедлайн истёк";
        const QString deadlineText = test.deadlineAt.isEmpty()
            ? "без дедлайна"
            : QString("дедлайн: %1").arg(test.deadlineAt);
        const QString attemptsText = test.maxAttempts == 0
            ? QString("попытки: %1 / без ограничения").arg(test.attemptsUsed)
            : QString("попытки: %1 из %2").arg(test.attemptsUsed).arg(test.maxAttempts);
        const QString passedText = test.passed
            ? QString("пройдено, лучший результат %1%").arg(QString::number(test.bestPercentage, 'f', 1))
            : "ещё не пройдено";
        const bool canOpen = (m_role == "Teacher" || m_role == "Admin") || (test.available && test.canAttempt);
        appendTestCard(
            m_testsList,
            test.id,
            test.title,
            m_role == "Student"
                ? QString("ID теста: %1  •  %2  •  %3  •  %4  •  %5")
                    .arg(test.id)
                    .arg(statusText, deadlineText, attemptsText, passedText)
                : QString("ID теста: %1  •  %2  •  %3  •  лимит попыток: %4")
                    .arg(test.id)
                    .arg(statusText, deadlineText)
                    .arg(test.maxAttempts == 0 ? "без ограничения" : QString::number(test.maxAttempts)),
            this,
            [this, test]() {
                emit testSelected(test.id, test.title);
            },
            canOpen);
        auto *cardWidget = m_testsList->itemWidget(m_testsList->item(m_testsList->count() - 1));
        if (auto *button = cardWidget ? cardWidget->findChild<QPushButton *>() : nullptr) {
            if (m_role == "Teacher" || m_role == "Admin") {
                button->setText("Открыть тест");
            } else if (!test.canAttempt) {
                button->setText("Попытки закончились");
            } else if (test.passed) {
                button->setText("Повторить тест");
            } else {
                button->setText("Перейти к тесту");
            }
        }
    }
    m_testsSummaryLabel->setText(QString::number(tests.size()));
    refreshOverview();
}

void CourseDetailsPage::setStudents(const QVector<CourseStudentData> &students)
{
    m_students = students;
    if (m_studentsList == nullptr) {
        return;
    }

    m_studentsList->clear();
    if (students.isEmpty()) {
        appendCard(m_studentsList, "Учеников пока нет", "На этот курс ещё никто не записался.");
    } else {
        for (const CourseStudentData &student : students) {
            appendCard(
                m_studentsList,
                student.login,
                QString("Группа: %1  •  общий прогресс: %2%  •  уроки: %3%  •  тесты: %4%")
                    .arg(student.groupName.isEmpty() ? "не указана" : student.groupName)
                    .arg(student.progress)
                    .arg(student.lessonProgress)
                    .arg(student.testProgress));
        }
    }
    refreshOverview();
}

void CourseDetailsPage::showLoadingState()
{
    m_lessons.clear();
    m_materials.clear();
    m_videos.clear();
    m_tests.clear();
    m_lessonsList->clear();
    m_materialsList->clear();
    m_videosList->clear();
    m_testsList->clear();
    if (m_studentsList != nullptr) {
        m_studentsList->clear();
    }

    appendCard(m_lessonsList, "Загружаем уроки...", "Получаем содержимое курса.");
    appendCard(m_materialsList, "Загружаем материалы...", "Подбираем дополнительные ресурсы.");
    appendCard(m_videosList, "Загружаем видео...", "Проверяем, есть ли видеоматериалы.");
    appendCard(m_testsList, "Загружаем тесты...", "Получаем тесты курса.");
    if (m_studentsList != nullptr) {
        appendCard(m_studentsList, "Загружаем учеников...", "Получаем список участников курса.");
    }
    m_lessonsSummaryLabel->setText("—");
    m_materialsSummaryLabel->setText("—");
    m_videosSummaryLabel->setText("—");
    m_testsSummaryLabel->setText("—");
    refreshOverview();
}

void CourseDetailsPage::setListItems(
    QListWidget *list,
    const QVector<QPair<QString, QString>> &items,
    const QString &emptyTitle,
    const QString &emptySubtitle)
{
    list->clear();

    if (items.isEmpty()) {
        appendCard(list, emptyTitle, emptySubtitle);
        return;
    }

    for (const auto &item : items) {
        appendCard(list, item.first, item.second);
    }
}

void CourseDetailsPage::refreshOverview()
{
    const QString courseTitle = m_course.id >= 0 && !m_course.title.isEmpty()
        ? m_course.title
        : "выбранного курса";

    if (m_role == "Teacher") {
        m_overviewHintLabel->setText(
            QString("Этот экран показывает учебную структуру %1 в режиме студента: уроки, материалы, видео и тесты.")
                .arg(courseTitle));
    } else if (m_role == "Admin") {
        m_overviewHintLabel->setText(
            QString("Административный обзор %1: здесь можно проверить структуру курса, материалы и тесты без student-flow.")
                .arg(courseTitle));
    } else {
        m_overviewHintLabel->setText(
            QString("Здесь собраны все элементы %1: сначала уроки и материалы, затем переход к тестам.")
                .arg(courseTitle));
    }

    if (m_role == "Student") {
        if (m_course.id < 0) {
            m_progressHintLabel->setText(
                "Конкретный курс покажет учебный маршрут: сколько там уроков, материалов и когда логично переходить к тестам.");
            return;
        }

        const bool hasLessons = !m_lessons.isEmpty();
        const bool hasResources = !m_materials.isEmpty() || !m_videos.isEmpty();
        const bool hasTests = !m_tests.isEmpty();
        const int completedLessons = std::count_if(m_lessons.cbegin(), m_lessons.cend(), [](const LessonData &lesson) {
            return lesson.completed;
        });

        m_progressHintLabel->setText(QString(
            "Маршрут по курсу: уроки — %1, материалы и видео — %2, тесты — %3.\n"
            "Уроки изучены: %4 из %5. Тестовый прогресс: %6% (%7 из %8 тестов зачтено). Следующий шаг: %9")
            .arg(hasLessons ? QString("доступны (%1)").arg(m_lessons.size()) : "ещё не добавлены")
            .arg(hasResources ? QString("доступны (%1)").arg(m_materials.size() + m_videos.size()) : "пока мало контента")
            .arg(hasTests ? QString("готовы (%1)").arg(m_tests.size()) : "пока нет тестов")
            .arg(completedLessons)
            .arg(m_lessons.size())
            .arg(m_course.progressPercent)
            .arg(m_course.passedTestsCount)
            .arg(m_course.testsCount)
            .arg(!hasLessons
                ? "курс ожидает наполнения уроками со стороны преподавателя."
                : !hasResources
                    ? "начать лучше с уроков, а материалы появятся по мере наполнения курса."
                    : hasTests
                        ? "рекомендуется изучить уроки и материалы, затем перейти к тестам."
                        : "сейчас лучше изучить содержание курса, тесты появятся позже."));
        return;
    }

    if (m_role == "Teacher") {
        m_progressHintLabel->setText(
            QString("Режим студента этого курса уже показывает: %1 уроков, %2 материалов/видео и %3 тестов. Это помогает быстро понять, как курс выглядит глазами ученика.")
                .arg(m_lessons.size())
                .arg(m_materials.size() + m_videos.size())
                .arg(m_tests.size()));
        return;
    }

    m_progressHintLabel->setText(
        QString("Системный обзор: в курсе сейчас %1 уроков, %2 материалов/видео и %3 тестов.")
            .arg(m_lessons.size())
            .arg(m_materials.size() + m_videos.size())
            .arg(m_tests.size()));
}
