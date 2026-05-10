#include "teachercoursebuilderpage.h"
#include "../ui/uistyles.h"

#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
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
#include <QAbstractItemView>
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

QLabel *createFieldLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName("moduleTitleLabel");
    return label;
}

QString statusWord(bool ready)
{
    return ready ? "готово" : "ожидает";
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
        "}"
        "QComboBox::drop-down {"
        " width: 34px;"
        " border: none;"
        " background: transparent;"
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
        " padding: 11px 20px;"
        " min-width: 112px;"
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

    m_stageChecklistLabel = new QLabel(overviewTab);
    m_stageChecklistLabel->setObjectName("sectionHintLabel");
    m_stageChecklistLabel->setWordWrap(true);
    m_stageChecklistLabel->setStyleSheet(
        "QLabel {"
        " background: #eff6ff;"
        " color: #1e3a8a;"
        " border: 1px solid #bfdbfe;"
        " border-radius: 16px;"
        " padding: 14px 16px;"
        " line-height: 1.4;"
        "}");

    overviewLayout->addLayout(summaryLayout);
    overviewLayout->addWidget(overviewNote);
    overviewLayout->addWidget(m_stageChecklistLabel);
    overviewLayout->addStretch();

    auto *lessonsTab = new QWidget(tabs);
    auto *lessonsLayout = new QHBoxLayout(lessonsTab);
    lessonsLayout->setContentsMargins(0, 0, 0, 0);
    lessonsLayout->setSpacing(14);

    QVBoxLayout *lessonsCardLayout = nullptr;
    auto *lessonsCard = createSectionCard("Уроки курса", lessonsTab, &lessonsCardLayout);
    m_lessonsList = new QListWidget(lessonsCard);
    m_lessonsList->setSpacing(10);
    m_lessonsList->setMinimumWidth(360);
    lessonsCardLayout->addWidget(m_lessonsList);

    QVBoxLayout *lessonFormLayout = nullptr;
    auto *lessonFormCard = createSectionCard("Добавить урок", lessonsTab, &lessonFormLayout);
    auto *lessonFormHint = new QLabel(
        "Сначала задай понятное название урока, затем добавь краткое содержание или основной учебный текст.",
        lessonFormCard);
    lessonFormHint->setObjectName("sectionHintLabel");
    lessonFormHint->setWordWrap(true);
    auto *lessonForm = new QFormLayout();
    lessonForm->setHorizontalSpacing(12);
    lessonForm->setVerticalSpacing(12);

    m_lessonTitleEdit = new QLineEdit(lessonFormCard);
    m_lessonTitleEdit->setPlaceholderText("Название урока");
    m_lessonTitleEdit->setStyleSheet(inputStyle);
    m_lessonTitleEdit->setMinimumHeight(48);

    m_lessonContentEdit = new QTextEdit(lessonFormCard);
    m_lessonContentEdit->setPlaceholderText("Текст, краткое описание или основное содержание урока");
    m_lessonContentEdit->setMinimumHeight(180);
    m_lessonContentEdit->setStyleSheet(inputStyle);

    m_addLessonButton = new QPushButton("Добавить урок", lessonFormCard);
    m_addLessonButton->setObjectName("cardAccentButton");
    m_loadLessonButton = new QPushButton("Загрузить урок в форму", lessonFormCard);
    m_loadLessonButton->setObjectName("cardGhostButton");
    m_updateLessonButton = new QPushButton("Сохранить изменения урока", lessonFormCard);
    m_updateLessonButton->setObjectName("cardAccentButton");
    m_deleteLessonButton = new QPushButton("Удалить урок", lessonFormCard);
    m_deleteLessonButton->setObjectName("cardDangerButton");

    auto *lessonPrimaryRow = new QHBoxLayout();
    lessonPrimaryRow->setSpacing(10);
    lessonPrimaryRow->addWidget(m_addLessonButton);
    lessonPrimaryRow->addWidget(m_loadLessonButton);
    lessonPrimaryRow->addStretch();

    auto *lessonDangerRow = new QHBoxLayout();
    lessonDangerRow->setSpacing(10);
    lessonDangerRow->addWidget(m_updateLessonButton);
    lessonDangerRow->addWidget(m_deleteLessonButton);
    lessonDangerRow->addStretch();

    lessonFormLayout->addWidget(lessonFormHint);
    lessonForm->addRow("Название", m_lessonTitleEdit);
    lessonForm->addRow("Контент", m_lessonContentEdit);
    lessonFormLayout->addLayout(lessonForm);
    lessonFormLayout->addLayout(lessonPrimaryRow);
    lessonFormLayout->addLayout(lessonDangerRow);
    lessonFormLayout->addStretch();

    lessonsLayout->addWidget(lessonsCard, 3);
    lessonsLayout->addWidget(lessonFormCard, 2);

    auto *materialsTab = new QWidget(tabs);
    auto *materialsLayout = new QHBoxLayout(materialsTab);
    materialsLayout->setContentsMargins(0, 0, 0, 0);
    materialsLayout->setSpacing(14);

    QVBoxLayout *materialsCardLayout = nullptr;
    auto *materialsCard = createSectionCard("Материалы урока", materialsTab, &materialsCardLayout);
    m_materialLessonCombo = ui_styles::createComboBox(materialsCard);
    ui_styles::applyComboBoxStyle(m_materialLessonCombo);
    m_materialsList = new QListWidget(materialsCard);
    m_materialsList->setSpacing(10);
    m_materialsList->setMinimumWidth(360);
    materialsCardLayout->addWidget(m_materialLessonCombo);
    materialsCardLayout->addWidget(m_materialsList);

    QVBoxLayout *materialFormLayout = nullptr;
    auto *materialFormCard = createSectionCard("Добавить материал", materialsTab, &materialFormLayout);
    auto *materialFormHint = new QLabel(
        "Материал всегда привязан к уроку. Для video добавляй ссылку, для text — краткий учебный контент.",
        materialFormCard);
    materialFormHint->setObjectName("sectionHintLabel");
    materialFormHint->setWordWrap(true);
    m_materialsGuardLabel = new QLabel(materialFormCard);
    m_materialsGuardLabel->setObjectName("sectionHintLabel");
    m_materialsGuardLabel->setWordWrap(true);
    m_materialsGuardLabel->setStyleSheet(
        "QLabel {"
        " background: #fff7ed;"
        " color: #9a3412;"
        " border: 1px solid #fed7aa;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        "}");
    auto *materialForm = new QFormLayout();
    materialForm->setHorizontalSpacing(12);
    materialForm->setVerticalSpacing(12);

    m_materialTitleEdit = new QLineEdit(materialFormCard);
    m_materialTitleEdit->setPlaceholderText("Например: Теория по указателям");
    m_materialTitleEdit->setStyleSheet(inputStyle);
    m_materialTitleEdit->setMinimumHeight(48);

    m_materialTypeCombo = ui_styles::createComboBox(materialFormCard);
    m_materialTypeCombo->addItems({"text", "video", "link"});
    ui_styles::applyComboBoxStyle(m_materialTypeCombo);

    m_materialContentEdit = new QTextEdit(materialFormCard);
    m_materialContentEdit->setPlaceholderText("Текст материала, ссылка на видео или полезный ресурс");
    m_materialContentEdit->setMinimumHeight(180);
    m_materialContentEdit->setStyleSheet(inputStyle);

    m_materialPreviewEdit = new QTextEdit(materialFormCard);
    m_materialPreviewEdit->setReadOnly(true);
    m_materialPreviewEdit->setMinimumHeight(120);
    m_materialPreviewEdit->setPlaceholderText("Здесь появится предпросмотр выбранного материала.");
    m_materialPreviewEdit->setStyleSheet(inputStyle + "QTextEdit { background-color: #f8fbff; }");

    m_addMaterialButton = new QPushButton("Добавить материал", materialFormCard);
    m_addMaterialButton->setObjectName("cardAccentButton");
    m_loadMaterialButton = new QPushButton("Загрузить материал в форму", materialFormCard);
    m_loadMaterialButton->setObjectName("cardGhostButton");
    m_updateMaterialButton = new QPushButton("Сохранить изменения материала", materialFormCard);
    m_updateMaterialButton->setObjectName("cardAccentButton");
    m_deleteMaterialButton = new QPushButton("Удалить материал", materialFormCard);
    m_deleteMaterialButton->setObjectName("cardDangerButton");

    auto *materialPrimaryRow = new QHBoxLayout();
    materialPrimaryRow->setSpacing(10);
    materialPrimaryRow->addWidget(m_addMaterialButton);
    materialPrimaryRow->addWidget(m_loadMaterialButton);
    materialPrimaryRow->addStretch();

    auto *materialDangerRow = new QHBoxLayout();
    materialDangerRow->setSpacing(10);
    materialDangerRow->addWidget(m_updateMaterialButton);
    materialDangerRow->addWidget(m_deleteMaterialButton);
    materialDangerRow->addStretch();

    materialFormLayout->addWidget(materialFormHint);
    materialFormLayout->addWidget(m_materialsGuardLabel);
    materialForm->addRow("Урок", m_materialLessonCombo);
    materialForm->addRow("Название", m_materialTitleEdit);
    materialForm->addRow("Тип", m_materialTypeCombo);
    materialForm->addRow("Содержимое", m_materialContentEdit);
    materialForm->addRow("Предпросмотр", m_materialPreviewEdit);
    materialFormLayout->addLayout(materialForm);
    materialFormLayout->addLayout(materialPrimaryRow);
    materialFormLayout->addLayout(materialDangerRow);
    materialFormLayout->addStretch();

    materialsLayout->addWidget(materialsCard, 3);
    materialsLayout->addWidget(materialFormCard, 2);

    auto *testsTab = new QWidget(tabs);
    auto *testsLayout = new QHBoxLayout(testsTab);
    testsLayout->setContentsMargins(0, 0, 0, 0);
    testsLayout->setSpacing(14);

    QVBoxLayout *testsColumnLayout = new QVBoxLayout();
    testsColumnLayout->setSpacing(14);

    QVBoxLayout *testsCardLayout = nullptr;
    auto *testsCard = createSectionCard("Тесты курса", testsTab, &testsCardLayout);
    m_testsList = new QListWidget(testsCard);
    m_testsList->setSpacing(10);
    m_testsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_testsList->setMinimumWidth(340);
    testsCardLayout->addWidget(m_testsList);
    testsColumnLayout->addWidget(testsCard);

    QVBoxLayout *questionsCardLayout = nullptr;
    auto *questionsCard = createSectionCard("Вопросы выбранного теста", testsTab, &questionsCardLayout);
    m_questionsList = new QListWidget(questionsCard);
    m_questionsList->setSpacing(10);
    m_questionsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_questionsList->setMinimumWidth(340);
    questionsCardLayout->addWidget(m_questionsList);
    testsColumnLayout->addWidget(questionsCard);

    auto *rightColumnLayout = new QVBoxLayout();
    rightColumnLayout->setSpacing(14);

    QVBoxLayout *testFormLayout = nullptr;
    auto *testFormCard = createSectionCard("1. Настрой тест", testsTab, &testFormLayout);
    auto *testFormHint = new QLabel(
        "Для быстрых правок можно работать здесь. Если вопросов уже много, удобнее открыть отдельный пошаговый редактор тестов.",
        testFormCard);
    testFormHint->setObjectName("sectionHintLabel");
    testFormHint->setWordWrap(true);
    auto *openEditorButton = new QPushButton("Перейти в пошаговый редактор тестов", testFormCard);
    openEditorButton->setObjectName("cardGhostButton");

    auto *testForm = new QFormLayout();
    testForm->setHorizontalSpacing(12);
    testForm->setVerticalSpacing(12);

    m_testTitleEdit = new QLineEdit(testFormCard);
    m_testTitleEdit->setPlaceholderText("Например: Финальный тест по теме");
    m_testTitleEdit->setStyleSheet(inputStyle);
    m_testTitleEdit->setMinimumHeight(48);

    m_addTestButton = new QPushButton("Создать тест", testFormCard);
    m_addTestButton->setObjectName("cardAccentButton");
    m_loadTestButton = new QPushButton("Загрузить тест в форму", testFormCard);
    m_loadTestButton->setObjectName("cardGhostButton");
    m_updateTestButton = new QPushButton("Сохранить изменения теста", testFormCard);
    m_updateTestButton->setObjectName("cardAccentButton");
    m_deleteTestButton = new QPushButton("Удалить тест", testFormCard);
    m_deleteTestButton->setObjectName("cardDangerButton");

    auto *testButtonsRow = new QHBoxLayout();
    testButtonsRow->setSpacing(10);
    testButtonsRow->addWidget(m_addTestButton);
    testButtonsRow->addWidget(m_loadTestButton);
    testButtonsRow->addStretch();

    auto *testDangerRow = new QHBoxLayout();
    testDangerRow->setSpacing(10);
    testDangerRow->addWidget(m_updateTestButton);
    testDangerRow->addWidget(m_deleteTestButton);
    testDangerRow->addStretch();

    testFormLayout->addWidget(testFormHint);
    testFormLayout->addWidget(openEditorButton, 0, Qt::AlignLeft);
    testForm->addRow("Название теста", m_testTitleEdit);
    testFormLayout->addLayout(testForm);
    testFormLayout->addLayout(testButtonsRow);
    testFormLayout->addLayout(testDangerRow);
    rightColumnLayout->addWidget(testFormCard);

    QVBoxLayout *questionSummaryLayout = nullptr;
    auto *questionSummaryCard = createSectionCard("2. Вопросы и сценарий теста", testsTab, &questionSummaryLayout);
    auto *questionDivider = new QLabel(
        "Внутри конструктора оставляем только быстрый обзор. Полное редактирование вопросов удобнее делать в отдельном пошаговом редакторе.",
        questionSummaryCard);
    questionDivider->setObjectName("sectionHintLabel");
    questionDivider->setWordWrap(true);
    m_questionsGuardLabel = new QLabel(questionSummaryCard);
    m_questionsGuardLabel->setObjectName("sectionHintLabel");
    m_questionsGuardLabel->setWordWrap(true);
    m_questionsGuardLabel->setStyleSheet(
        "QLabel {"
        " background: #fff7ed;"
        " color: #9a3412;"
        " border: 1px solid #fed7aa;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        "}");

    m_questionTestCombo = ui_styles::createComboBox(questionSummaryCard);
    ui_styles::applyComboBoxStyle(m_questionTestCombo);
    m_questionTestCombo->setMinimumHeight(46);

    m_questionEditorSummaryLabel = new QLabel(questionSummaryCard);
    m_questionEditorSummaryLabel->setObjectName("sectionHintLabel");
    m_questionEditorSummaryLabel->setWordWrap(true);

    auto *openQuestionEditorButton = new QPushButton("Открыть отдельный редактор вопросов", questionSummaryCard);
    openQuestionEditorButton->setObjectName("cardAccentButton");

    questionSummaryLayout->addWidget(questionDivider);
    questionSummaryLayout->addWidget(m_questionsGuardLabel);
    questionSummaryLayout->addWidget(createFieldLabel("Тест для предпросмотра вопросов", questionSummaryCard));
    questionSummaryLayout->addWidget(m_questionTestCombo);
    questionSummaryLayout->addWidget(m_questionEditorSummaryLabel);
    questionSummaryLayout->addWidget(openQuestionEditorButton, 0, Qt::AlignLeft);
    questionSummaryLayout->addStretch();

    auto *questionFormCard = new QFrame(testsTab);
    questionFormCard->hide();
    auto *questionFormLayout = new QVBoxLayout(questionFormCard);
    questionFormLayout->setContentsMargins(0, 0, 0, 0);
    questionFormLayout->setSpacing(10);

    m_questionTextEdit = new QTextEdit(questionFormCard);
    m_questionTextEdit->setPlaceholderText("Текст вопроса");
    m_questionTextEdit->setMinimumHeight(110);
    m_questionTextEdit->setStyleSheet(inputStyle);

    m_optionOneEdit = new QLineEdit(questionFormCard);
    m_optionTwoEdit = new QLineEdit(questionFormCard);
    m_optionThreeEdit = new QLineEdit(questionFormCard);
    m_optionFourEdit = new QLineEdit(questionFormCard);
    for (QLineEdit *edit : {m_optionOneEdit, m_optionTwoEdit, m_optionThreeEdit, m_optionFourEdit}) {
        edit->setPlaceholderText("Вариант ответа");
        edit->setStyleSheet(inputStyle);
        edit->setMinimumHeight(44);
    }

    m_correctOptionCombo = ui_styles::createComboBox(questionFormCard);
    m_correctOptionCombo->addItems({"Вариант 1", "Вариант 2", "Вариант 3", "Вариант 4"});
    ui_styles::applyComboBoxStyle(m_correctOptionCombo);
    m_correctOptionCombo->setMinimumHeight(46);

    m_optionOrderCombo = ui_styles::createComboBox(questionFormCard);
    m_optionOrderCombo->addItems({"Вариант 1", "Вариант 2", "Вариант 3", "Вариант 4"});
    ui_styles::applyComboBoxStyle(m_optionOrderCombo);
    m_optionOrderCombo->setMinimumHeight(46);

    m_moveOptionUpButton = new QPushButton("Поднять вариант выше", questionFormCard);
    m_moveOptionUpButton->setObjectName("cardGhostButton");
    m_moveOptionDownButton = new QPushButton("Опустить вариант ниже", questionFormCard);
    m_moveOptionDownButton->setObjectName("cardGhostButton");

    m_addQuestionButton = new QPushButton("Сохранить вопрос", questionFormCard);
    m_addQuestionButton->setObjectName("cardAccentButton");
    m_loadQuestionButton = new QPushButton("Загрузить вопрос в форму", questionFormCard);
    m_loadQuestionButton->setObjectName("cardGhostButton");
    m_updateQuestionButton = new QPushButton("Сохранить изменения вопроса", questionFormCard);
    m_updateQuestionButton->setObjectName("cardAccentButton");
    m_deleteQuestionButton = new QPushButton("Удалить вопрос", questionFormCard);
    m_deleteQuestionButton->setObjectName("cardDangerButton");

    auto *optionsGrid = new QGridLayout();
    optionsGrid->setHorizontalSpacing(12);
    optionsGrid->setVerticalSpacing(10);
    optionsGrid->addWidget(createFieldLabel("Вариант 1", questionFormCard), 0, 0);
    optionsGrid->addWidget(createFieldLabel("Вариант 2", questionFormCard), 0, 1);
    optionsGrid->addWidget(m_optionOneEdit, 1, 0);
    optionsGrid->addWidget(m_optionTwoEdit, 1, 1);
    optionsGrid->addWidget(createFieldLabel("Вариант 3", questionFormCard), 2, 0);
    optionsGrid->addWidget(createFieldLabel("Вариант 4", questionFormCard), 2, 1);
    optionsGrid->addWidget(m_optionThreeEdit, 3, 0);
    optionsGrid->addWidget(m_optionFourEdit, 3, 1);

    auto *answerSettingsColumn = new QVBoxLayout();
    answerSettingsColumn->setSpacing(10);
    answerSettingsColumn->addWidget(createFieldLabel("Правильный ответ", questionFormCard));
    answerSettingsColumn->addWidget(m_correctOptionCombo);
    answerSettingsColumn->addWidget(createFieldLabel("Какой вариант переставить", questionFormCard));
    answerSettingsColumn->addWidget(m_optionOrderCombo);
    auto *optionOrderRow = new QHBoxLayout();
    optionOrderRow->setSpacing(10);
    optionOrderRow->addWidget(m_moveOptionUpButton);
    optionOrderRow->addWidget(m_moveOptionDownButton);
    optionOrderRow->addStretch();
    auto *questionPrimaryRow = new QHBoxLayout();
    questionPrimaryRow->setSpacing(10);
    questionPrimaryRow->addWidget(m_addQuestionButton);
    questionPrimaryRow->addWidget(m_loadQuestionButton);
    questionPrimaryRow->addStretch();

    auto *questionDangerRow = new QHBoxLayout();
    questionDangerRow->setSpacing(10);
    questionDangerRow->addWidget(m_updateQuestionButton);
    questionDangerRow->addWidget(m_deleteQuestionButton);
    questionDangerRow->addStretch();

    questionFormLayout->addWidget(questionDivider);
    questionFormLayout->addWidget(m_questionsGuardLabel);
    questionFormLayout->addWidget(createFieldLabel("Тест, к которому относится вопрос", questionFormCard));
    questionFormLayout->addWidget(m_questionTestCombo);
    questionFormLayout->addWidget(createFieldLabel("Формулировка вопроса", questionFormCard));
    questionFormLayout->addWidget(m_questionTextEdit);
    questionFormLayout->addWidget(createFieldLabel("Варианты ответа", questionFormCard));
    questionFormLayout->addLayout(optionsGrid);
    questionFormLayout->addLayout(answerSettingsColumn);
    questionFormLayout->addLayout(optionOrderRow);
    questionFormLayout->addLayout(questionPrimaryRow);
    questionFormLayout->addLayout(questionDangerRow);
    questionFormLayout->addStretch();

    testsLayout->addLayout(testsColumnLayout, 3);
    rightColumnLayout->addWidget(questionSummaryCard);
    testsLayout->addLayout(rightColumnLayout, 2);

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
    connect(m_lessonsList, &QListWidget::itemSelectionChanged, this, [this]() { updateActionState(); });
    connect(m_materialLessonCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
        refreshMaterialsList();
        updateActionState();
    });
    connect(m_materialsList, &QListWidget::itemSelectionChanged, this, [this]() { updateActionState(); });
    connect(m_materialsList, &QListWidget::itemSelectionChanged, this, [this]() { refreshMaterialPreview(); });
    connect(m_questionTestCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        Q_UNUSED(index);
        refreshQuestionsList();
        updateActionState();
        if (selectedManagedTestId() >= 0) {
            emit testSelectedForQuestions(selectedManagedTestId());
        }
    });
    connect(m_testsList, &QListWidget::itemSelectionChanged, this, [this]() {
        QListWidgetItem *currentItem = m_testsList->currentItem();
        if (!currentItem) {
            updateActionState();
            return;
        }

        const int testId = currentItem->data(Qt::UserRole).toInt();
        if (testId < 0) {
            updateActionState();
            return;
        }

        for (const auto &test : std::as_const(m_tests)) {
            if (test.id == testId) {
                populateTestDraft(test);
                break;
            }
        }

        for (int i = 0; i < m_questionTestCombo->count(); ++i) {
            if (m_questionTestCombo->itemData(i).toInt() == testId) {
                m_questionTestCombo->setCurrentIndex(i);
                break;
            }
        }

        showMessage("Тест выбран. Ниже можно посмотреть вопросы или перейти в отдельный редактор.", false);
        updateActionState();
    });
    connect(m_questionsList, &QListWidget::itemSelectionChanged, this, [this]() {
        const int questionId = selectedQuestionId();
        if (questionId < 0) {
            updateActionState();
            return;
        }

        for (const auto &question : std::as_const(m_questions)) {
            if (question.id == questionId) {
                populateQuestionDraft(question);
                break;
            }
        }
        updateActionState();
    });
    connect(m_materialTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_materialContentEdit, &QTextEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_testTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_questionTextEdit, &QTextEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionOneEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionTwoEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionThreeEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionFourEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionOrderCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() { updateActionState(); });
    connect(m_moveOptionUpButton, &QPushButton::clicked, this, [this]() {
        const int index = m_optionOrderCombo->currentIndex();
        if (index <= 0) {
            return;
        }

        QStringList options = questionOptionTexts();
        options.swapItemsAt(index, index - 1);
        setQuestionOptionTexts(options);

        int correctIndex = m_correctOptionCombo->currentIndex();
        if (correctIndex == index) {
            m_correctOptionCombo->setCurrentIndex(index - 1);
        } else if (correctIndex == index - 1) {
            m_correctOptionCombo->setCurrentIndex(index);
        }

        m_optionOrderCombo->setCurrentIndex(index - 1);
        showMessage("Порядок вариантов обновлён. Не забудь сохранить вопрос.", false);
    });
    connect(m_moveOptionDownButton, &QPushButton::clicked, this, [this]() {
        const int index = m_optionOrderCombo->currentIndex();
        if (index < 0 || index >= 3) {
            return;
        }

        QStringList options = questionOptionTexts();
        options.swapItemsAt(index, index + 1);
        setQuestionOptionTexts(options);

        int correctIndex = m_correctOptionCombo->currentIndex();
        if (correctIndex == index) {
            m_correctOptionCombo->setCurrentIndex(index + 1);
        } else if (correctIndex == index + 1) {
            m_correctOptionCombo->setCurrentIndex(index);
        }

        m_optionOrderCombo->setCurrentIndex(index + 1);
        showMessage("Порядок вариантов обновлён. Не забудь сохранить вопрос.", false);
    });

    connect(m_addLessonButton, &QPushButton::clicked, this, [this]() {
        emit createLessonRequested(
            m_course.id,
            m_lessonTitleEdit->text().trimmed(),
            m_lessonContentEdit->toPlainText().trimmed());
    });
    connect(m_loadLessonButton, &QPushButton::clicked, this, [this]() {
        const int lessonId = selectedLessonListId();
        for (const auto& lesson : std::as_const(m_lessons)) {
            if (lesson.id == lessonId) {
                populateLessonDraft(lesson);
                showMessage("Урок загружен в форму для редактирования.", false);
                return;
            }
        }
    });
    connect(m_updateLessonButton, &QPushButton::clicked, this, [this]() {
        emit updateLessonRequested(
            m_editingLessonId,
            m_lessonTitleEdit->text().trimmed(),
            m_lessonContentEdit->toPlainText().trimmed());
    });
    connect(m_deleteLessonButton, &QPushButton::clicked, this, [this]() {
        emit deleteLessonRequested(m_editingLessonId);
    });
    connect(m_addMaterialButton, &QPushButton::clicked, this, [this]() {
        emit createMaterialRequested(
            selectedLessonId(),
            m_materialTitleEdit->text().trimmed(),
            m_materialTypeCombo->currentText(),
            m_materialContentEdit->toPlainText().trimmed());
    });
    connect(m_loadMaterialButton, &QPushButton::clicked, this, [this]() {
        const int materialId = selectedMaterialId();
        for (const auto& material : std::as_const(m_materials)) {
            if (material.id == materialId) {
                populateMaterialDraft(material);
                showMessage("Материал загружен в форму для редактирования.", false);
                return;
            }
        }
    });
    connect(m_updateMaterialButton, &QPushButton::clicked, this, [this]() {
        emit updateMaterialRequested(
            m_editingMaterialId,
            m_materialTitleEdit->text().trimmed(),
            m_materialTypeCombo->currentText(),
            m_materialContentEdit->toPlainText().trimmed());
    });
    connect(m_deleteMaterialButton, &QPushButton::clicked, this, [this]() {
        emit deleteMaterialRequested(m_editingMaterialId);
    });
    connect(m_addTestButton, &QPushButton::clicked, this, [this]() {
        emit createTestRequested(m_course.id, m_testTitleEdit->text().trimmed());
    });
    connect(m_loadTestButton, &QPushButton::clicked, this, [this]() {
        const int testId = selectedManagedTestId();
        for (const auto& test : std::as_const(m_tests)) {
            if (test.id == testId) {
                populateTestDraft(test);
                showMessage("Тест загружен в форму для редактирования.", false);
                return;
            }
        }
    });
    connect(m_updateTestButton, &QPushButton::clicked, this, [this]() {
        emit updateTestRequested(m_editingTestId, m_testTitleEdit->text().trimmed());
    });
    connect(m_deleteTestButton, &QPushButton::clicked, this, [this]() {
        emit deleteTestRequested(m_editingTestId);
    });
    connect(openEditorButton, &QPushButton::clicked, this, &TeacherCourseBuilderPage::openDedicatedTestEditorRequested);
    connect(openQuestionEditorButton, &QPushButton::clicked, this, &TeacherCourseBuilderPage::openDedicatedTestEditorRequested);
    connect(m_addQuestionButton, &QPushButton::clicked, this, [this]() {
        QStringList options{
            m_optionOneEdit->text().trimmed(),
            m_optionTwoEdit->text().trimmed(),
            m_optionThreeEdit->text().trimmed(),
            m_optionFourEdit->text().trimmed()
        };

        emit createQuestionRequested(
            selectedManagedTestId(),
            m_questionTextEdit->toPlainText().trimmed(),
            options,
            m_correctOptionCombo->currentIndex());
    });
    connect(m_loadQuestionButton, &QPushButton::clicked, this, [this]() {
        const int questionId = selectedQuestionId();
        for (const auto& question : std::as_const(m_questions)) {
            if (question.id == questionId) {
                populateQuestionDraft(question);
                showMessage("Вопрос загружен в форму для редактирования.", false);
                return;
            }
        }
    });
    connect(m_updateQuestionButton, &QPushButton::clicked, this, [this]() {
        QStringList options{
            m_optionOneEdit->text().trimmed(),
            m_optionTwoEdit->text().trimmed(),
            m_optionThreeEdit->text().trimmed(),
            m_optionFourEdit->text().trimmed()
        };
        emit updateQuestionRequested(
            m_editingQuestionId,
            m_questionTextEdit->toPlainText().trimmed(),
            options,
            m_correctOptionCombo->currentIndex());
    });
    connect(m_deleteQuestionButton, &QPushButton::clicked, this, [this]() {
        emit deleteQuestionRequested(m_editingQuestionId);
    });

    clearBuilder();
}

void TeacherCourseBuilderPage::clearBuilder()
{
    m_course = CourseData{};
    m_lessons.clear();
    m_materials.clear();
    m_tests.clear();
    m_questions.clear();
    m_editingLessonId = -1;
    m_editingMaterialId = -1;
    m_editingTestId = -1;
    m_editingQuestionId = -1;

    m_courseTitleLabel->setText("Конструктор курса");
    m_courseDescriptionLabel->setText("Выбери курс во вкладке \"Мои курсы\", чтобы начать оформление структуры курса.");
    showMessage("Сейчас конструктор ждёт выбранный курс.", false);

    clearLessonDraft();
    clearMaterialDraft();
    clearTestDraft();
    clearQuestionDraft();
    refreshOverview();
    refreshLessonsList();
    refreshLessonSelector();
    refreshMaterialsList();
    refreshTestSelector();
    refreshTestsList();
    refreshQuestionsList();
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
    refreshTestSelector();
    refreshTestsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setQuestions(const QVector<QuestionData> &questions)
{
    m_questions = questions;
    refreshQuestionsList();
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
    m_editingLessonId = -1;
    m_lessonTitleEdit->clear();
    m_lessonContentEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::clearMaterialDraft()
{
    m_editingMaterialId = -1;
    m_materialTitleEdit->clear();
    m_materialContentEdit->clear();
    m_materialTypeCombo->setCurrentIndex(0);
    m_materialPreviewEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::clearTestDraft()
{
    m_editingTestId = -1;
    m_testTitleEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::clearQuestionDraft()
{
    m_editingQuestionId = -1;
    m_questionTextEdit->clear();
    m_optionOneEdit->clear();
    m_optionTwoEdit->clear();
    m_optionThreeEdit->clear();
    m_optionFourEdit->clear();
    m_correctOptionCombo->setCurrentIndex(0);
    m_optionOrderCombo->setCurrentIndex(0);
    updateActionState();
}

int TeacherCourseBuilderPage::currentManagedTestId() const
{
    return selectedManagedTestId();
}

void TeacherCourseBuilderPage::populateTestDraft(const TestData &test)
{
    m_editingTestId = test.id;
    m_testTitleEdit->setText(test.title);
    updateActionState();
}

void TeacherCourseBuilderPage::populateQuestionDraft(const QuestionData &question)
{
    m_editingQuestionId = question.id;
    m_questionTextEdit->setText(question.text);
    m_optionOneEdit->setText(question.options.size() > 0 ? question.options[0].text : QString());
    m_optionTwoEdit->setText(question.options.size() > 1 ? question.options[1].text : QString());
    m_optionThreeEdit->setText(question.options.size() > 2 ? question.options[2].text : QString());
    m_optionFourEdit->setText(question.options.size() > 3 ? question.options[3].text : QString());

    int correctIndex = 0;
    for (int i = 0; i < question.options.size(); ++i) {
        if (question.options[i].id == question.correctAnswerId) {
            correctIndex = i;
            break;
        }
    }
    m_correctOptionCombo->setCurrentIndex(correctIndex);
    updateActionState();
}

void TeacherCourseBuilderPage::populateLessonDraft(const LessonData &lesson)
{
    m_editingLessonId = lesson.id;
    m_lessonTitleEdit->setText(lesson.title);
    m_lessonContentEdit->setText(lesson.content);
    updateActionState();
}

void TeacherCourseBuilderPage::populateMaterialDraft(const MaterialData &material)
{
    m_editingMaterialId = material.id;
    for (int i = 0; i < m_materialLessonCombo->count(); ++i) {
        if (m_materialLessonCombo->itemData(i).toInt() == material.lessonId) {
            m_materialLessonCombo->setCurrentIndex(i);
            break;
        }
    }
    m_materialTitleEdit->setText(material.title);
    const int typeIndex = m_materialTypeCombo->findText(material.type);
    m_materialTypeCombo->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
    m_materialContentEdit->setText(material.content);
    refreshMaterialPreview();
    updateActionState();
}

void TeacherCourseBuilderPage::refreshOverview()
{
    m_lessonsSummaryLabel->setText(QString::number(m_lessons.size()));
    m_materialsSummaryLabel->setText(QString::number(m_materials.size()));
    m_testsSummaryLabel->setText(QString::number(m_tests.size()));

    const bool hasCourse = m_course.id >= 0;
    const bool lessonsReady = !m_lessons.isEmpty();
    const bool materialsReady = !m_materials.isEmpty();
    const bool testsReady = !m_tests.isEmpty();
    const bool builderReady = lessonsReady && materialsReady && testsReady;

    if (!hasCourse) {
        m_stageChecklistLabel->setText(
            "Шаг 1. Выбери курс во вкладке \"Мои курсы\".\n"
            "Шаг 2. Добавь уроки, затем привяжи материалы.\n"
            "Шаг 3. Настрой тесты и только после этого переходи к полноценной проверке курса.");
        return;
    }

    m_stageChecklistLabel->setText(QString(
        "Шаг 1. Уроки — %1 (%2)\n"
        "Шаг 2. Материалы — %3 (%4)\n"
        "Шаг 3. Тесты — %5 (%6)\n"
        "Итог: %7")
        .arg(statusWord(lessonsReady))
        .arg(lessonsReady ? QString("создано %1").arg(m_lessons.size()) : QString("сначала добавь первый урок"))
        .arg(statusWord(materialsReady))
        .arg(materialsReady ? QString("добавлено %1").arg(m_materials.size()) : QString("привяжи материалы к урокам"))
        .arg(statusWord(testsReady))
        .arg(testsReady ? QString("создано %1").arg(m_tests.size()) : QString("настрой хотя бы один тест"))
        .arg(builderReady
            ? "структура курса уже выглядит полной, можно идти в студентов и аналитику."
            : "конструктор ещё не завершён: добавь недостающие блоки, чтобы курс стал рабочим."));
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
        m_lessonsList->item(m_lessonsList->count() - 1)->setData(Qt::UserRole, lesson.id);
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
        m_materialsList->item(m_materialsList->count() - 1)->setData(Qt::UserRole, material.id);
    }

    if (!hasItems) {
        appendBuilderCard(
            m_materialsList,
            "Материалов для этого урока пока нет",
            "Выбери тип материала и добавь первый текст, ссылку или видео.");
    }

    refreshMaterialPreview();
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
        m_testsList->item(m_testsList->count() - 1)->setData(Qt::UserRole, test.id);
    }
}

void TeacherCourseBuilderPage::refreshTestSelector()
{
    const int currentTestId = selectedManagedTestId();

    m_questionTestCombo->blockSignals(true);
    m_questionTestCombo->clear();

    for (const TestData &test : std::as_const(m_tests)) {
        m_questionTestCombo->addItem(test.title, test.id);
    }

    if (!m_tests.isEmpty()) {
        int selectedIndex = 0;
        for (int i = 0; i < m_questionTestCombo->count(); ++i) {
            if (m_questionTestCombo->itemData(i).toInt() == currentTestId) {
                selectedIndex = i;
                break;
            }
        }
        m_questionTestCombo->setCurrentIndex(selectedIndex);
    }

    m_questionTestCombo->blockSignals(false);
}

void TeacherCourseBuilderPage::refreshQuestionsList()
{
    m_questionsList->clear();

    const int testId = selectedManagedTestId();
    if (testId < 0) {
        appendBuilderCard(
            m_questionsList,
            "Сначала создай тест",
            "После этого выбери его в форме справа и начни добавлять вопросы.");
        return;
    }

    if (m_questions.isEmpty()) {
        appendBuilderCard(
            m_questionsList,
            "Вопросов пока нет",
            "Добавь первый вопрос и четыре варианта ответа через форму справа.");
        return;
    }

    for (const QuestionData &question : std::as_const(m_questions)) {
        QString correctText = "Правильный ответ пока не определён";
        for (const auto &option : question.options) {
            if (option.id == question.correctAnswerId) {
                correctText = QString("Правильный ответ: %1").arg(option.text);
                break;
            }
        }

        appendBuilderCard(
            m_questionsList,
            question.text,
            correctText);
        m_questionsList->item(m_questionsList->count() - 1)->setData(Qt::UserRole, question.id);
    }
}

void TeacherCourseBuilderPage::refreshMaterialPreview()
{
    const int materialId = selectedMaterialId();
    if (materialId < 0) {
        m_materialPreviewEdit->setPlainText("Выбери материал в списке слева, чтобы увидеть его содержимое.");
        return;
    }

    for (const MaterialData &material : std::as_const(m_materials)) {
        if (material.id != materialId) {
            continue;
        }

        const QString preview = QString("Тип: %1\n\n%2")
                                    .arg(material.type.isEmpty() ? "text" : material.type)
                                    .arg(material.content.isEmpty() ? "Материал пока пуст." : material.content);
        m_materialPreviewEdit->setPlainText(preview);
        return;
    }

    m_materialPreviewEdit->setPlainText("Предпросмотр материала недоступен.");
}

QStringList TeacherCourseBuilderPage::questionOptionTexts() const
{
    return {
        m_optionOneEdit->text(),
        m_optionTwoEdit->text(),
        m_optionThreeEdit->text(),
        m_optionFourEdit->text()
    };
}

void TeacherCourseBuilderPage::setQuestionOptionTexts(const QStringList &options)
{
    m_optionOneEdit->setText(options.value(0));
    m_optionTwoEdit->setText(options.value(1));
    m_optionThreeEdit->setText(options.value(2));
    m_optionFourEdit->setText(options.value(3));
}

int TeacherCourseBuilderPage::selectedLessonId() const
{
    if (m_materialLessonCombo->count() == 0) {
        return -1;
    }

    return m_materialLessonCombo->currentData().toInt();
}

int TeacherCourseBuilderPage::selectedLessonListId() const
{
    QListWidgetItem *item = m_lessonsList->currentItem();
    if (!item) {
        return -1;
    }

    const QVariant lessonId = item->data(Qt::UserRole);
    if (!lessonId.isValid()) {
        return -1;
    }

    return lessonId.toInt();
}

int TeacherCourseBuilderPage::selectedMaterialId() const
{
    QListWidgetItem *item = m_materialsList->currentItem();
    if (!item) {
        return -1;
    }

    const QVariant materialId = item->data(Qt::UserRole);
    if (!materialId.isValid()) {
        return -1;
    }

    return materialId.toInt();
}

int TeacherCourseBuilderPage::selectedManagedTestId() const
{
    if (m_questionTestCombo->count() == 0) {
        return -1;
    }

    return m_questionTestCombo->currentData().toInt();
}

int TeacherCourseBuilderPage::selectedQuestionId() const
{
    QListWidgetItem *item = m_questionsList->currentItem();
    if (!item) {
        return -1;
    }

    const QVariant questionId = item->data(Qt::UserRole);
    if (!questionId.isValid()) {
        return -1;
    }

    return questionId.toInt();
}

void TeacherCourseBuilderPage::updateActionState()
{
    const bool hasCourse = m_course.id >= 0;
    const bool hasLessons = !m_lessons.isEmpty();
    const bool hasTests = !m_tests.isEmpty();
    const bool hasSelectedLesson = selectedLessonId() >= 0;
    const bool hasSelectedTest = selectedManagedTestId() >= 0;
    const bool materialFormEnabled = hasCourse && hasLessons;
    const bool questionFormEnabled = hasCourse && hasTests && hasSelectedTest;

    m_addLessonButton->setEnabled(
        hasCourse
        && !m_lessonTitleEdit->text().trimmed().isEmpty()
        && !m_lessonContentEdit->toPlainText().trimmed().isEmpty());
    m_loadLessonButton->setEnabled(selectedLessonListId() >= 0);
    m_updateLessonButton->setEnabled(
        hasCourse
        && m_editingLessonId >= 0
        && !m_lessonTitleEdit->text().trimmed().isEmpty()
        && !m_lessonContentEdit->toPlainText().trimmed().isEmpty());
    m_deleteLessonButton->setEnabled(m_editingLessonId >= 0);

    m_addMaterialButton->setEnabled(
        hasCourse
        && hasLessons
        && hasSelectedLesson
        && !m_materialTitleEdit->text().trimmed().isEmpty()
        && !m_materialContentEdit->toPlainText().trimmed().isEmpty());
    m_loadMaterialButton->setEnabled(selectedMaterialId() >= 0);
    m_updateMaterialButton->setEnabled(
        hasCourse
        && hasLessons
        && m_editingMaterialId >= 0
        && hasSelectedLesson
        && !m_materialTitleEdit->text().trimmed().isEmpty()
        && !m_materialContentEdit->toPlainText().trimmed().isEmpty());
    m_deleteMaterialButton->setEnabled(m_editingMaterialId >= 0);

    m_addTestButton->setEnabled(
        hasCourse
        && !m_testTitleEdit->text().trimmed().isEmpty());
    m_loadTestButton->setEnabled(hasTests && selectedManagedTestId() >= 0);
    m_updateTestButton->setEnabled(
        hasCourse
        && m_editingTestId >= 0
        && !m_testTitleEdit->text().trimmed().isEmpty());
    m_deleteTestButton->setEnabled(m_editingTestId >= 0);

    m_addQuestionButton->setEnabled(
        hasCourse
        && hasTests
        && hasSelectedTest
        && !m_questionTextEdit->toPlainText().trimmed().isEmpty()
        && !m_optionOneEdit->text().trimmed().isEmpty()
        && !m_optionTwoEdit->text().trimmed().isEmpty()
        && !m_optionThreeEdit->text().trimmed().isEmpty()
        && !m_optionFourEdit->text().trimmed().isEmpty());
    m_loadQuestionButton->setEnabled(selectedQuestionId() >= 0);
    m_updateQuestionButton->setEnabled(
        hasCourse
        && m_editingQuestionId >= 0
        && hasTests
        && hasSelectedTest
        && !m_questionTextEdit->toPlainText().trimmed().isEmpty()
        && !m_optionOneEdit->text().trimmed().isEmpty()
        && !m_optionTwoEdit->text().trimmed().isEmpty()
        && !m_optionThreeEdit->text().trimmed().isEmpty()
        && !m_optionFourEdit->text().trimmed().isEmpty());
    m_deleteQuestionButton->setEnabled(m_editingQuestionId >= 0);
    m_moveOptionUpButton->setEnabled(questionFormEnabled && m_optionOrderCombo->currentIndex() > 0);
    m_moveOptionDownButton->setEnabled(questionFormEnabled && m_optionOrderCombo->currentIndex() >= 0 && m_optionOrderCombo->currentIndex() < 3);

    m_materialLessonCombo->setEnabled(materialFormEnabled);
    m_materialTitleEdit->setEnabled(materialFormEnabled);
    m_materialTypeCombo->setEnabled(materialFormEnabled);
    m_materialContentEdit->setEnabled(materialFormEnabled);
    m_materialPreviewEdit->setEnabled(true);
    m_materialsGuardLabel->setVisible(!materialFormEnabled || !hasSelectedLesson);
    if (!hasCourse) {
        m_materialsGuardLabel->setText("Сначала выбери курс, а затем переходи к наполнению уроков материалами.");
    } else if (!hasLessons) {
        m_materialsGuardLabel->setText("Сначала создай хотя бы один урок. После этого здесь станет доступно добавление материалов.");
    } else if (!hasSelectedLesson) {
        m_materialsGuardLabel->setText("Выбери урок в выпадающем списке, чтобы привязать к нему материал.");
    }

    m_questionTestCombo->setEnabled(hasCourse && hasTests);
    m_questionTextEdit->setEnabled(questionFormEnabled);
    m_optionOneEdit->setEnabled(questionFormEnabled);
    m_optionTwoEdit->setEnabled(questionFormEnabled);
    m_optionThreeEdit->setEnabled(questionFormEnabled);
    m_optionFourEdit->setEnabled(questionFormEnabled);
    m_correctOptionCombo->setEnabled(questionFormEnabled);
    m_optionOrderCombo->setEnabled(questionFormEnabled);
    m_questionsGuardLabel->setVisible(!questionFormEnabled);
    if (!hasCourse) {
        m_questionsGuardLabel->setText("Сначала выбери курс, затем настрой тесты и только после этого переходи к вопросам.");
    } else if (!hasTests) {
        m_questionsGuardLabel->setText("Сначала создай хотя бы один тест. После этого здесь можно будет собирать вопросы и правильные ответы.");
    } else if (!hasSelectedTest) {
        m_questionsGuardLabel->setText("Выбери тест в списке или в выпадающем поле, чтобы начать наполнять его вопросами.");
    }

    if (!hasCourse) {
        m_questionEditorSummaryLabel->setText("Сначала выбери курс. После этого здесь появится краткая сводка по выбранному тесту и переход к отдельному редактору.");
    } else if (!hasTests) {
        m_questionEditorSummaryLabel->setText("У курса пока нет тестов. Создай хотя бы один тест в верхнем блоке, и тогда можно будет переходить к вопросам.");
    } else if (!hasSelectedTest) {
        m_questionEditorSummaryLabel->setText("Выбери тест в списке слева. Здесь появится краткая сводка, а полное редактирование вопросов откроется в отдельном окне.");
    } else {
        const int questionCount = m_questions.size();
        m_questionEditorSummaryLabel->setText(
            questionCount == 0
                ? "У выбранного теста пока нет вопросов. Открой отдельный редактор и создай первый вопрос там."
                : QString("У выбранного теста сейчас %1 вопросов. В builder оставляем только обзор, а глубокое редактирование ведём в отдельном редакторе.")
                    .arg(questionCount));
    }
}
