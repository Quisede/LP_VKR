#include "coursedetailspage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
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

    auto *modulesLayout = new QHBoxLayout();
    modulesLayout->setSpacing(14);

    auto *lessonsCard = new QFrame(pageCard);
    lessonsCard->setObjectName("moduleCard");
    auto *lessonsLayout = new QVBoxLayout(lessonsCard);
    lessonsLayout->setContentsMargins(18, 18, 18, 18);
    auto *lessonsTitle = new QLabel("Уроки", lessonsCard);
    lessonsTitle->setObjectName("moduleTitleLabel");
    m_lessonsList = new QListWidget(lessonsCard);
    lessonsLayout->addWidget(lessonsTitle);
    lessonsLayout->addWidget(m_lessonsList);

    auto *materialsCard = new QFrame(pageCard);
    materialsCard->setObjectName("moduleCard_2");
    auto *materialsLayout = new QVBoxLayout(materialsCard);
    materialsLayout->setContentsMargins(18, 18, 18, 18);
    auto *materialsTitle = new QLabel("Материалы", materialsCard);
    materialsTitle->setObjectName("moduleTitleLabel");
    m_materialsList = new QListWidget(materialsCard);
    materialsLayout->addWidget(materialsTitle);
    materialsLayout->addWidget(m_materialsList);

    auto *videosCard = new QFrame(pageCard);
    videosCard->setObjectName("moduleCard_3");
    auto *videosLayout = new QVBoxLayout(videosCard);
    videosLayout->setContentsMargins(18, 18, 18, 18);
    auto *videosTitle = new QLabel("Видео", videosCard);
    videosTitle->setObjectName("moduleTitleLabel");
    m_videosList = new QListWidget(videosCard);
    videosLayout->addWidget(videosTitle);
    videosLayout->addWidget(m_videosList);

    auto *testsCard = new QFrame(pageCard);
    testsCard->setObjectName("moduleCard_4");
    auto *testsLayout = new QVBoxLayout(testsCard);
    testsLayout->setContentsMargins(18, 18, 18, 18);
    auto *testsTitle = new QLabel("Тесты", testsCard);
    testsTitle->setObjectName("moduleTitleLabel");
    m_testsList = new QListWidget(testsCard);
    testsLayout->addWidget(testsTitle);
    testsLayout->addWidget(m_testsList);

    for (QListWidget *list : {m_lessonsList, m_materialsList, m_videosList, m_testsList}) {
        list->setSpacing(10);
    }

    modulesLayout->addWidget(lessonsCard);
    modulesLayout->addWidget(materialsCard);
    modulesLayout->addWidget(videosCard);
    modulesLayout->addWidget(testsCard);

    pageLayout->addLayout(topLayout);
    pageLayout->addWidget(m_titleLabel);
    pageLayout->addWidget(m_descriptionLabel);
    pageLayout->addLayout(modulesLayout);

    rootLayout->addWidget(pageCard);

    connect(backButton, &QPushButton::clicked, this, &CourseDetailsPage::backRequested);
    connect(m_primaryActionButton, &QPushButton::clicked, this, &CourseDetailsPage::enrollRequested);
}

void CourseDetailsPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher") {
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
        m_titleLabel->setText(m_role == "Teacher" ? "Курс не выбран" : "Курс без названия");
        m_descriptionLabel->setText(
            m_role == "Teacher"
                ? "Выбери курс во вкладке \"Мои курсы\", чтобы открыть конструктор курса."
                : "Для этого курса пока нет подробного описания.");
        return;
    }

    m_titleLabel->setText(course.title.isEmpty() ? "Курс без названия" : course.title);
    m_descriptionLabel->setText(
        course.description.isEmpty()
            ? "Для этого курса пока нет подробного описания."
            : course.description);
}

void CourseDetailsPage::setLessons(const QVector<LessonData> &lessons)
{
    QVector<QPair<QString, QString>> items;
    for (const auto &lesson : lessons) {
        items.push_back({lesson.title, lesson.content});
    }
    setListItems(m_lessonsList, items, "Уроков пока нет", "Для курса ещё не добавлены уроки.");
}

void CourseDetailsPage::setMaterials(const QVector<MaterialData> &materials, const QVector<MaterialData> &videos)
{
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
}

void CourseDetailsPage::setTests(const QVector<TestData> &tests)
{
    m_testsList->clear();
    if (tests.isEmpty()) {
        appendCard(m_testsList, "Тестов пока нет", "Для этого курса ещё не добавили тесты.");
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
            button->setText(m_role == "Teacher" ? "Открыть тест" : "Начать тест");
        }
    }
}

void CourseDetailsPage::showLoadingState()
{
    m_lessonsList->clear();
    m_materialsList->clear();
    m_videosList->clear();
    m_testsList->clear();

    appendCard(m_lessonsList, "Загружаем уроки...", "Получаем содержимое курса.");
    appendCard(m_materialsList, "Загружаем материалы...", "Подбираем дополнительные ресурсы.");
    appendCard(m_videosList, "Загружаем видео...", "Проверяем, есть ли видеоматериалы.");
    appendCard(m_testsList, "Загружаем тесты...", "Получаем тесты курса.");
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
