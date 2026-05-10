#include "coursedetailspage.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTabWidget>
#include <QVBoxLayout>
#include <functional>

namespace {
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

void appendTestCard(
    QListWidget *list,
    int testId,
    const QString &title,
    const QString &subtitle,
    QObject *context,
    const std::function<void()> &onStart)
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 132));
    item->setData(Qt::UserRole, testId);
    item->setData(Qt::UserRole + 1, title);

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(10);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *subtitleLabel = new QLabel(subtitle, card);
    subtitleLabel->setObjectName("courseCardDescriptionLabel");
    subtitleLabel->setWordWrap(true);

    auto *startButton = new QPushButton("Начать тест", card);
    startButton->setObjectName("cardAccentButton");

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addWidget(startButton, 0, Qt::AlignLeft);

    QObject::connect(startButton, &QPushButton::clicked, context, onStart);

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
        "Открой раздел курсов и нажми на карточку нужного курса.",
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

    m_sectionsTabs->addTab(overviewTab, "Обзор");
    m_sectionsTabs->addTab(lessonsTab, "Уроки");
    m_sectionsTabs->addTab(materialsTab, "Материалы");
    m_sectionsTabs->addTab(videosTab, "Видео");
    m_sectionsTabs->addTab(testsTab, "Тесты");

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
    } else {
        m_primaryActionButton->show();
        m_primaryActionButton->setText("Записаться на курс");
    }
}

void CourseDetailsPage::setCourse(const CourseData &course)
{
    m_course = course;
    if (course.id < 0) {
        m_titleLabel->setText((m_role == "Teacher" || m_role == "Admin") ? "Курс не выбран" : "Курс без названия");
        m_descriptionLabel->setText(
            (m_role == "Teacher" || m_role == "Admin")
                ? "Выбери курс во вкладке \"Мои курсы\", чтобы открыть конструктор курса."
                : "Для этого курса пока нет подробного описания.");
        refreshOverview();
        return;
    }

    m_titleLabel->setText(course.title.isEmpty() ? "Курс без названия" : course.title);
    m_descriptionLabel->setText(
        course.description.isEmpty()
            ? "Для этого курса пока нет подробного описания."
            : course.description);
    refreshOverview();
}

void CourseDetailsPage::setLessons(const QVector<LessonData> &lessons)
{
    m_lessons = lessons;
    QVector<QPair<QString, QString>> items;
    for (const auto &lesson : lessons) {
        items.push_back({lesson.title, lesson.content});
    }
    setListItems(m_lessonsList, items, "Уроков пока нет", "Для курса ещё не добавлены уроки.");
    m_lessonsSummaryLabel->setText(QString::number(lessons.size()));
    refreshOverview();
}

void CourseDetailsPage::setMaterials(const QVector<MaterialData> &materials, const QVector<MaterialData> &videos)
{
    m_materials = materials;
    m_videos = videos;
    QVector<QPair<QString, QString>> materialItems;
    for (const auto &material : materials) {
        materialItems.push_back({material.title, material.content});
    }
    setListItems(m_materialsList, materialItems, "Материалов пока нет", "У этого курса нет обычных материалов.");

    QVector<QPair<QString, QString>> videoItems;
    for (const auto &video : videos) {
        videoItems.push_back({video.title, video.content});
    }
    setListItems(m_videosList, videoItems, "Видео пока нет", "У этого курса нет видеоматериалов.");
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
        appendTestCard(
            m_testsList,
            test.id,
            test.title,
            QString("ID теста: %1").arg(test.id),
            this,
            [this, test]() {
                emit testSelected(test.id, test.title);
            });
        auto *cardWidget = m_testsList->itemWidget(m_testsList->item(m_testsList->count() - 1));
        if (auto *button = cardWidget ? cardWidget->findChild<QPushButton *>() : nullptr) {
            button->setText((m_role == "Teacher" || m_role == "Admin") ? "Открыть тест" : "Начать тест");
        }
    }
    m_testsSummaryLabel->setText(QString::number(tests.size()));
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

    appendCard(m_lessonsList, "Загружаем уроки...", "Получаем содержимое курса.");
    appendCard(m_materialsList, "Загружаем материалы...", "Подбираем дополнительные ресурсы.");
    appendCard(m_videosList, "Загружаем видео...", "Проверяем, есть ли видеоматериалы.");
    appendCard(m_testsList, "Загружаем тесты...", "Получаем тесты курса.");
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
            QString("Этот экран показывает учебную структуру %1 в student-view: уроки, материалы, видео и тесты.")
                .arg(courseTitle));
    } else if (m_role == "Admin") {
        m_overviewHintLabel->setText(
            QString("Административный обзор %1: здесь можно проверить структуру курса, материалы и тесты без student-flow.")
                .arg(courseTitle));
    } else {
        m_overviewHintLabel->setText(
            QString("Здесь собраны все элементы %1: сначала посмотри уроки и материалы, затем переходи к тестам.")
                .arg(courseTitle));
    }

    if (m_role == "Student") {
        if (m_course.id < 0) {
            m_progressHintLabel->setText(
                "Открой конкретный курс, чтобы увидеть его учебный маршрут: сколько там уроков, материалов и когда логично переходить к тестам.");
            return;
        }

        const bool hasLessons = !m_lessons.isEmpty();
        const bool hasResources = !m_materials.isEmpty() || !m_videos.isEmpty();
        const bool hasTests = !m_tests.isEmpty();

        m_progressHintLabel->setText(QString(
            "Маршрут по курсу: уроки — %1, материалы и видео — %2, тесты — %3.\n"
            "Следующий шаг: %4")
            .arg(hasLessons ? QString("доступны (%1)").arg(m_lessons.size()) : "ещё не добавлены")
            .arg(hasResources ? QString("доступны (%1)").arg(m_materials.size() + m_videos.size()) : "пока мало контента")
            .arg(hasTests ? QString("готовы (%1)").arg(m_tests.size()) : "пока нет тестов")
            .arg(!hasLessons
                ? "подожди, пока преподаватель наполнит курс уроками."
                : !hasResources
                    ? "начни с уроков, а материалы появятся по мере наполнения курса."
                    : hasTests
                        ? "посмотри уроки и материалы, затем переходи к тестам."
                        : "сейчас лучше изучить содержание курса, тесты появятся позже."));
        return;
    }

    if (m_role == "Teacher") {
        m_progressHintLabel->setText(
            QString("Student-view этого курса уже показывает: %1 уроков, %2 материалов/видео и %3 тестов. Это помогает быстро понять, как курс выглядит глазами ученика.")
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
