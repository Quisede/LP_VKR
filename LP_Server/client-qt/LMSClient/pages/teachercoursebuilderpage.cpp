#include "teachercoursebuilderpage.h"

#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTabBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <utility>

namespace {

void appendBuilderCard(QListWidget *list, const QString &title, const QString &subtitle)
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 92));

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
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

QFrame *createSectionCard(const QString &title, QWidget *parent, QVBoxLayout **contentLayout)
{
    auto *card = new QFrame(parent);
    card->setObjectName("moduleCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("moduleTitleLabel");

    layout->addWidget(titleLabel);
    *contentLayout = layout;
    return card;
}

}

TeacherCourseBuilderPage::TeacherCourseBuilderPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("pageCard");

    auto *layout = new QVBoxLayout(pageCard);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    const QString inputStyle =
        "QLineEdit, QTextEdit, QComboBox {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " font-size: 14px;"
        "}";

    m_courseTitleLabel = new QLabel("Конструктор курса", pageCard);
    m_courseTitleLabel->setObjectName("sectionTitleLabel");

    m_courseDescriptionLabel = new QLabel(
        "Выбери курс во вкладке \"Мои курсы\", чтобы начать оформление структуры курса.",
        pageCard);
    m_courseDescriptionLabel->setObjectName("sectionHintLabel");
    m_courseDescriptionLabel->setWordWrap(true);

    m_messageLabel = new QLabel("Здесь можно добавлять уроки, материалы и тесты для выбранного курса.", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);

    auto *tabs = new QTabWidget(pageCard);
    tabs->setDocumentMode(true);
    tabs->setElideMode(Qt::ElideNone);
    tabs->tabBar()->setExpanding(false);
    tabs->tabBar()->setDrawBase(false);
    tabs->setStyleSheet(
        "QTabWidget::pane {"
        " border: 1px solid #dbe4f0;"
        " border-radius: 18px;"
        " margin-top: 14px;"
        " background: #f8fbff;"
        " padding: 18px 18px 12px 18px;"
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
        " margin-bottom: 0px;"
        "}"
        "QTabBar::tab:selected {"
        " background: #2563eb;"
        " color: #ffffff;"
        "}"
        "QTabBar::tab:hover:!selected {"
        " background: #cbd5e1;"
        "}");

    auto *overviewTab = new QWidget(tabs);
    auto *overviewLayout = new QVBoxLayout(overviewTab);
    overviewLayout->setContentsMargins(0, 0, 0, 0);
    overviewLayout->setSpacing(14);

    auto *summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(14);

    auto createSummaryCard = [overviewTab](const QString &title, QLabel **valueLabel) {
        auto *card = new QFrame(overviewTab);
        card->setObjectName("moduleCard");
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
    summaryLayout->addWidget(createSummaryCard("Тесты", &m_testsSummaryLabel));

    auto *overviewNote = new QLabel(
        "Собери структуру курса по шагам: сначала уроки, затем привяжи к ним материалы и после этого добавь тесты.",
        overviewTab);
    overviewNote->setObjectName("sectionHintLabel");
    overviewNote->setWordWrap(true);

    overviewLayout->addLayout(summaryLayout);
    overviewLayout->addWidget(overviewNote);
    overviewLayout->addStretch();

    auto *lessonsTab = new QWidget(tabs);
    auto *lessonsLayout = new QHBoxLayout(lessonsTab);
    lessonsLayout->setContentsMargins(0, 0, 0, 0);
    lessonsLayout->setSpacing(14);

    QVBoxLayout *lessonsCardLayout = nullptr;
    auto *lessonsCard = createSectionCard("Уроки курса", lessonsTab, &lessonsCardLayout);
    m_lessonsList = new QListWidget(lessonsCard);
    m_lessonsList->setSpacing(10);
    lessonsCardLayout->addWidget(m_lessonsList);

    QVBoxLayout *lessonFormLayout = nullptr;
    auto *lessonFormCard = createSectionCard("Добавить урок", lessonsTab, &lessonFormLayout);
    auto *lessonForm = new QFormLayout();
    lessonForm->setHorizontalSpacing(12);
    lessonForm->setVerticalSpacing(12);

    m_lessonTitleEdit = new QLineEdit(lessonFormCard);
    m_lessonTitleEdit->setPlaceholderText("Название урока");
    m_lessonTitleEdit->setStyleSheet(inputStyle);

    m_lessonContentEdit = new QTextEdit(lessonFormCard);
    m_lessonContentEdit->setPlaceholderText("Текст, краткое описание или основное содержание урока");
    m_lessonContentEdit->setMinimumHeight(180);
    m_lessonContentEdit->setStyleSheet(inputStyle);

    m_addLessonButton = new QPushButton("Добавить урок", lessonFormCard);
    m_addLessonButton->setObjectName("cardAccentButton");

    lessonForm->addRow("Название", m_lessonTitleEdit);
    lessonForm->addRow("Контент", m_lessonContentEdit);
    lessonForm->addRow("", m_addLessonButton);
    lessonFormLayout->addLayout(lessonForm);
    lessonFormLayout->addStretch();

    lessonsLayout->addWidget(lessonsCard, 3);
    lessonsLayout->addWidget(lessonFormCard, 2);

    auto *materialsTab = new QWidget(tabs);
    auto *materialsLayout = new QHBoxLayout(materialsTab);
    materialsLayout->setContentsMargins(0, 0, 0, 0);
    materialsLayout->setSpacing(14);

    QVBoxLayout *materialsCardLayout = nullptr;
    auto *materialsCard = createSectionCard("Материалы урока", materialsTab, &materialsCardLayout);
    m_materialLessonCombo = new QComboBox(materialsCard);
    m_materialLessonCombo->setStyleSheet(inputStyle);
    m_materialsList = new QListWidget(materialsCard);
    m_materialsList->setSpacing(10);
    materialsCardLayout->addWidget(m_materialLessonCombo);
    materialsCardLayout->addWidget(m_materialsList);

    QVBoxLayout *materialFormLayout = nullptr;
    auto *materialFormCard = createSectionCard("Добавить материал", materialsTab, &materialFormLayout);
    auto *materialForm = new QFormLayout();
    materialForm->setHorizontalSpacing(12);
    materialForm->setVerticalSpacing(12);

    m_materialTitleEdit = new QLineEdit(materialFormCard);
    m_materialTitleEdit->setPlaceholderText("Например: Теория по указателям");
    m_materialTitleEdit->setStyleSheet(inputStyle);

    m_materialTypeCombo = new QComboBox(materialFormCard);
    m_materialTypeCombo->addItems({"text", "video", "link"});
    m_materialTypeCombo->setStyleSheet(inputStyle);

    m_materialContentEdit = new QTextEdit(materialFormCard);
    m_materialContentEdit->setPlaceholderText("Текст материала, ссылка на видео или полезный ресурс");
    m_materialContentEdit->setMinimumHeight(180);
    m_materialContentEdit->setStyleSheet(inputStyle);

    m_addMaterialButton = new QPushButton("Добавить материал", materialFormCard);
    m_addMaterialButton->setObjectName("cardAccentButton");

    materialForm->addRow("Урок", m_materialLessonCombo);
    materialForm->addRow("Название", m_materialTitleEdit);
    materialForm->addRow("Тип", m_materialTypeCombo);
    materialForm->addRow("Содержимое", m_materialContentEdit);
    materialForm->addRow("", m_addMaterialButton);
    materialFormLayout->addLayout(materialForm);
    materialFormLayout->addStretch();

    materialsLayout->addWidget(materialsCard, 3);
    materialsLayout->addWidget(materialFormCard, 2);

    auto *testsTab = new QWidget(tabs);
    auto *testsLayout = new QHBoxLayout(testsTab);
    testsLayout->setContentsMargins(0, 0, 0, 0);
    testsLayout->setSpacing(14);

    QVBoxLayout *testsCardLayout = nullptr;
    auto *testsCard = createSectionCard("Тесты курса", testsTab, &testsCardLayout);
    m_testsList = new QListWidget(testsCard);
    m_testsList->setSpacing(10);
    testsCardLayout->addWidget(m_testsList);

    QVBoxLayout *testFormLayout = nullptr;
    auto *testFormCard = createSectionCard("Создать тест", testsTab, &testFormLayout);
    auto *testForm = new QFormLayout();
    testForm->setHorizontalSpacing(12);
    testForm->setVerticalSpacing(12);

    m_testTitleEdit = new QLineEdit(testFormCard);
    m_testTitleEdit->setPlaceholderText("Например: Финальный тест по теме");
    m_testTitleEdit->setStyleSheet(inputStyle);

    m_addTestButton = new QPushButton("Создать тест", testFormCard);
    m_addTestButton->setObjectName("cardAccentButton");

    testForm->addRow("Название теста", m_testTitleEdit);
    testForm->addRow("", m_addTestButton);
    testFormLayout->addLayout(testForm);
    testFormLayout->addStretch();

    testsLayout->addWidget(testsCard, 3);
    testsLayout->addWidget(testFormCard, 2);

    tabs->addTab(overviewTab, "Обзор");
    tabs->addTab(lessonsTab, "Уроки");
    tabs->addTab(materialsTab, "Материалы");
    tabs->addTab(testsTab, "Тесты");

    layout->addWidget(m_courseTitleLabel);
    layout->addWidget(m_courseDescriptionLabel);
    layout->addWidget(m_messageLabel);
    layout->addWidget(tabs);

    rootLayout->addWidget(pageCard);

    connect(m_lessonTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_lessonContentEdit, &QTextEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_materialLessonCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
        refreshMaterialsList();
        updateActionState();
    });
    connect(m_materialTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_materialContentEdit, &QTextEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_testTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });

    connect(m_addLessonButton, &QPushButton::clicked, this, [this]() {
        emit createLessonRequested(
            m_course.id,
            m_lessonTitleEdit->text().trimmed(),
            m_lessonContentEdit->toPlainText().trimmed());
    });
    connect(m_addMaterialButton, &QPushButton::clicked, this, [this]() {
        emit createMaterialRequested(
            selectedLessonId(),
            m_materialTitleEdit->text().trimmed(),
            m_materialTypeCombo->currentText(),
            m_materialContentEdit->toPlainText().trimmed());
    });
    connect(m_addTestButton, &QPushButton::clicked, this, [this]() {
        emit createTestRequested(m_course.id, m_testTitleEdit->text().trimmed());
    });

    clearBuilder();
}

void TeacherCourseBuilderPage::clearBuilder()
{
    m_course = CourseData{};
    m_lessons.clear();
    m_materials.clear();
    m_tests.clear();

    m_courseTitleLabel->setText("Конструктор курса");
    m_courseDescriptionLabel->setText("Выбери курс во вкладке \"Мои курсы\", чтобы начать оформление структуры курса.");
    showMessage("Сейчас конструктор ждёт выбранный курс.", false);

    clearLessonDraft();
    clearMaterialDraft();
    clearTestDraft();
    refreshOverview();
    refreshLessonsList();
    refreshLessonSelector();
    refreshMaterialsList();
    refreshTestsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setCourse(const CourseData &course)
{
    m_course = course;
    m_courseTitleLabel->setText(course.title.isEmpty() ? "Курс без названия" : course.title);
    m_courseDescriptionLabel->setText(
        course.description.isEmpty()
            ? "У курса пока нет описания. Добавь структуру ниже."
            : course.description);
    refreshOverview();
    updateActionState();
}

void TeacherCourseBuilderPage::setLessons(const QVector<LessonData> &lessons)
{
    m_lessons = lessons;
    refreshOverview();
    refreshLessonsList();
    refreshLessonSelector();
    refreshMaterialsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setMaterials(const QVector<MaterialData> &materials)
{
    m_materials = materials;
    refreshOverview();
    refreshMaterialsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setTests(const QVector<TestData> &tests)
{
    m_tests = tests;
    refreshOverview();
    refreshTestsList();
    updateActionState();
}

void TeacherCourseBuilderPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

void TeacherCourseBuilderPage::clearLessonDraft()
{
    m_lessonTitleEdit->clear();
    m_lessonContentEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::clearMaterialDraft()
{
    m_materialTitleEdit->clear();
    m_materialContentEdit->clear();
    m_materialTypeCombo->setCurrentIndex(0);
    updateActionState();
}

void TeacherCourseBuilderPage::clearTestDraft()
{
    m_testTitleEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::refreshOverview()
{
    m_lessonsSummaryLabel->setText(QString::number(m_lessons.size()));
    m_materialsSummaryLabel->setText(QString::number(m_materials.size()));
    m_testsSummaryLabel->setText(QString::number(m_tests.size()));
}

void TeacherCourseBuilderPage::refreshLessonsList()
{
    m_lessonsList->clear();

    if (m_lessons.isEmpty()) {
        appendBuilderCard(
            m_lessonsList,
            "Уроков пока нет",
            "Добавь первый урок через форму справа, и он сразу появится в структуре курса.");
        return;
    }

    for (const LessonData &lesson : std::as_const(m_lessons)) {
        appendBuilderCard(
            m_lessonsList,
            lesson.title,
            lesson.content.isEmpty() ? "Контент урока пока пуст" : lesson.content);
    }
}

void TeacherCourseBuilderPage::refreshLessonSelector()
{
    const int currentLesson = selectedLessonId();

    m_materialLessonCombo->blockSignals(true);
    m_materialLessonCombo->clear();

    for (const LessonData &lesson : std::as_const(m_lessons)) {
        m_materialLessonCombo->addItem(lesson.title, lesson.id);
    }

    if (!m_lessons.isEmpty()) {
        int selectedIndex = 0;
        for (int i = 0; i < m_materialLessonCombo->count(); ++i) {
            if (m_materialLessonCombo->itemData(i).toInt() == currentLesson) {
                selectedIndex = i;
                break;
            }
        }
        m_materialLessonCombo->setCurrentIndex(selectedIndex);
    }

    m_materialLessonCombo->blockSignals(false);
}

void TeacherCourseBuilderPage::refreshMaterialsList()
{
    m_materialsList->clear();

    const int lessonId = selectedLessonId();
    if (lessonId < 0) {
        appendBuilderCard(
            m_materialsList,
            "Сначала добавь урок",
            "Материалы привязываются к урокам, поэтому сначала создай хотя бы один урок.");
        return;
    }

    bool hasItems = false;
    for (const MaterialData &material : std::as_const(m_materials)) {
        if (material.lessonId != lessonId) {
            continue;
        }

        hasItems = true;
        appendBuilderCard(
            m_materialsList,
            QString("%1 (%2)").arg(material.title, material.type),
            material.content);
    }

    if (!hasItems) {
        appendBuilderCard(
            m_materialsList,
            "Материалов для этого урока пока нет",
            "Выбери тип материала и добавь первый текст, ссылку или видео.");
    }
}

void TeacherCourseBuilderPage::refreshTestsList()
{
    m_testsList->clear();

    if (m_tests.isEmpty()) {
        appendBuilderCard(
            m_testsList,
            "Тестов пока нет",
            "Создай первый тест курса, чтобы позже перейти к редактору вопросов.");
        return;
    }

    for (const TestData &test : std::as_const(m_tests)) {
        appendBuilderCard(
            m_testsList,
            test.title,
            QString("ID теста: %1").arg(test.id));
    }
}

int TeacherCourseBuilderPage::selectedLessonId() const
{
    if (m_materialLessonCombo->count() == 0) {
        return -1;
    }

    return m_materialLessonCombo->currentData().toInt();
}

void TeacherCourseBuilderPage::updateActionState()
{
    const bool hasCourse = m_course.id >= 0;
    const bool hasLessons = !m_lessons.isEmpty();

    m_addLessonButton->setEnabled(
        hasCourse
        && !m_lessonTitleEdit->text().trimmed().isEmpty()
        && !m_lessonContentEdit->toPlainText().trimmed().isEmpty());

    m_addMaterialButton->setEnabled(
        hasCourse
        && hasLessons
        && selectedLessonId() >= 0
        && !m_materialTitleEdit->text().trimmed().isEmpty()
        && !m_materialContentEdit->toPlainText().trimmed().isEmpty());

    m_addTestButton->setEnabled(
        hasCourse
        && !m_testTitleEdit->text().trimmed().isEmpty());
}
