#include "teachertesteditorpage.h"

#include "../ui/uistyles.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

namespace {

void appendEditorCard(QListWidget *list, const QString &title, const QString &subtitle)
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

void applyTransparentListStyle(QListWidget *list)
{
    list->setFrameShape(QFrame::NoFrame);
    list->setAttribute(Qt::WA_StyledBackground, true);
    list->viewport()->setAttribute(Qt::WA_StyledBackground, true);
    list->setStyleSheet(
        "QListWidget { background: transparent; border: none; outline: none; }"
        "QListWidget::item { background: transparent; border: none; margin: 0; padding: 0; }"
        "QListWidget::item:hover, QListWidget::item:selected { background: transparent; }");
    list->viewport()->setStyleSheet("background: transparent;");
}

}

TeacherTestEditorPage::TeacherTestEditorPage(QWidget *parent)
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

    auto *backButton = new QPushButton("Назад к конструктору", pageCard);
    backButton->setObjectName("backToCoursesButton");

    m_courseTitleLabel = new QLabel("Редактор тестов", pageCard);
    m_courseTitleLabel->setObjectName("sectionTitleLabel");

    m_courseDescriptionLabel = new QLabel(
        "Здесь удобно создавать тесты, редактировать вопросы и управлять вариантами ответов отдельно от общего конструктора.",
        pageCard);
    m_courseDescriptionLabel->setObjectName("sectionHintLabel");
    m_courseDescriptionLabel->setWordWrap(true);

    m_messageLabel = new QLabel("Сначала нужно выбрать курс в конструкторе и открыть редактор тестов.", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);

    auto *summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(14);
    auto createSummaryCard = [pageCard](const QString &title, QLabel **valueLabel) {
        auto *card = new QFrame(pageCard);
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
    summaryLayout->addWidget(createSummaryCard("Тесты", &m_testsSummaryLabel));
    summaryLayout->addWidget(createSummaryCard("Вопросы", &m_questionsSummaryLabel));

    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(14);

    QVBoxLayout *testsCardLayout = nullptr;
    auto *testsCard = createSectionCard("Тесты курса", pageCard, &testsCardLayout);
    m_testsList = new QListWidget(testsCard);
    m_testsList->setSpacing(10);
    m_testsList->setSelectionMode(QAbstractItemView::SingleSelection);
    applyTransparentListStyle(m_testsList);
    testsCardLayout->addWidget(m_testsList);

    QVBoxLayout *questionsCardLayout = nullptr;
    auto *questionsCard = createSectionCard("Вопросы выбранного теста", pageCard, &questionsCardLayout);
    m_questionsList = new QListWidget(questionsCard);
    m_questionsList->setSpacing(10);
    m_questionsList->setSelectionMode(QAbstractItemView::SingleSelection);
    applyTransparentListStyle(m_questionsList);
    questionsCardLayout->addWidget(m_questionsList);

    auto *leftColumn = new QVBoxLayout();
    leftColumn->setSpacing(14);
    leftColumn->addWidget(testsCard);
    leftColumn->addWidget(questionsCard);

    m_editorTabs = new QTabWidget(pageCard);
    m_editorTabs->setDocumentMode(true);
    m_editorTabs->tabBar()->setExpanding(false);
    m_editorTabs->tabBar()->setDrawBase(false);
    m_editorTabs->setStyleSheet(
        "QTabWidget::pane {"
        " border: 1px solid #dbe4f0;"
        " border-radius: 18px;"
        " margin-top: 12px;"
        " background: #f8fbff;"
        " padding: 14px;"
        "}"
        "QTabWidget::tab-bar { alignment: left; left: 10px; }"
        "QTabBar::tab {"
        " background: #e2e8f0;"
        " color: #334155;"
        " border: none;"
        " border-top-left-radius: 12px;"
        " border-top-right-radius: 12px;"
        " padding: 10px 12px;"
        " min-width: 88px;"
        " font-weight: 600;"
        " margin-right: 6px;"
        "}"
        "QTabBar::tab:selected {"
        " background: #2563eb;"
        " color: #ffffff;"
        "}");

    QVBoxLayout *testFormLayout = nullptr;
    auto *testFormCard = createSectionCard("1. Управление тестом", pageCard, &testFormLayout);
    auto *testHint = new QLabel(
        "Слева нужно выбрать тест курса. Здесь можно быстро создать новый тест, переименовать выбранный или удалить его.",
        testFormCard);
    testHint->setObjectName("sectionHintLabel");
    testHint->setWordWrap(true);
    m_testTitleEdit = new QLineEdit(testFormCard);
    m_testTitleEdit->setPlaceholderText("Например: Финальный тест по теме");
    m_testTitleEdit->setMinimumHeight(48);
    m_testTitleEdit->setStyleSheet(inputStyle);

    m_testMaxAttemptsSpin = new QSpinBox(testFormCard);
    m_testMaxAttemptsSpin->setRange(0, 20);
    m_testMaxAttemptsSpin->setSpecialValueText("Без ограничения");
    m_testMaxAttemptsSpin->setMinimumHeight(46);
    m_testMaxAttemptsSpin->setStyleSheet(
        "QSpinBox { background: #ffffff; color: #0f172a; border: 1px solid #dbe4f0; border-radius: 14px; padding: 8px 12px; }"
        "QSpinBox:focus { border-color: #2563eb; }");

    m_testTimeLimitSpin = new QSpinBox(testFormCard);
    m_testTimeLimitSpin->setRange(1, 300);
    m_testTimeLimitSpin->setSuffix(" мин.");
    m_testTimeLimitSpin->setValue(30);
    m_testTimeLimitSpin->setMinimumHeight(46);
    m_testTimeLimitSpin->setStyleSheet(
        "QSpinBox { background: #ffffff; color: #0f172a; border: 1px solid #dbe4f0; border-radius: 14px; padding: 8px 12px; }"
        "QSpinBox:focus { border-color: #2563eb; }");

    m_addTestButton = new QPushButton("Создать тест", testFormCard);
    m_addTestButton->setObjectName("cardAccentButton");
    m_loadTestButton = new QPushButton("Редактировать выбранный тест", testFormCard);
    m_loadTestButton->setObjectName("cardGhostButton");
    m_updateTestButton = new QPushButton("Сохранить изменения теста", testFormCard);
    m_updateTestButton->setObjectName("cardAccentButton");
    m_deleteTestButton = new QPushButton("Удалить тест", testFormCard);
    m_deleteTestButton->setObjectName("cardDangerButton");

    auto *testPrimaryRow = new QHBoxLayout();
    testPrimaryRow->setSpacing(10);
    testPrimaryRow->addWidget(m_addTestButton);
    testPrimaryRow->addWidget(m_loadTestButton);
    testPrimaryRow->addStretch();

    auto *testDangerRow = new QHBoxLayout();
    testDangerRow->setSpacing(10);
    testDangerRow->addWidget(m_updateTestButton);
    testDangerRow->addWidget(m_deleteTestButton);
    testDangerRow->addStretch();

    testFormLayout->addWidget(testHint);
    testFormLayout->addWidget(createFieldLabel("Название теста", testFormCard));
    testFormLayout->addWidget(m_testTitleEdit);
    testFormLayout->addWidget(createFieldLabel("Количество попыток", testFormCard));
    testFormLayout->addWidget(m_testMaxAttemptsSpin);
    testFormLayout->addWidget(createFieldLabel("Время на прохождение", testFormCard));
    testFormLayout->addWidget(m_testTimeLimitSpin);
    testFormLayout->addLayout(testPrimaryRow);
    testFormLayout->addLayout(testDangerRow);

    QVBoxLayout *questionFormLayout = nullptr;
    auto *questionFormCard = createSectionCard("2. Формулировка вопроса", pageCard, &questionFormLayout);
    auto *questionHint = new QLabel(
        "Выберите тест, к которому относится вопрос, и задайте понятную формулировку. Варианты ответа настраиваются следующим шагом.",
        questionFormCard);
    questionHint->setObjectName("sectionHintLabel");
    questionHint->setWordWrap(true);

    m_questionTestCombo = ui_styles::createComboBox(questionFormCard);
    ui_styles::applyComboBoxStyle(m_questionTestCombo);
    m_questionTestCombo->setMinimumHeight(46);

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
        edit->setMinimumHeight(44);
        edit->setStyleSheet(inputStyle);
    }

    m_correctOptionCombo = ui_styles::createComboBox(questionFormCard);
    m_correctOptionCombo->addItems({"Вариант 1", "Вариант 2", "Вариант 3", "Вариант 4"});
    ui_styles::applyComboBoxStyle(m_correctOptionCombo);

    m_optionOrderCombo = ui_styles::createComboBox(questionFormCard);
    m_optionOrderCombo->addItems({"Вариант 1", "Вариант 2", "Вариант 3", "Вариант 4"});
    ui_styles::applyComboBoxStyle(m_optionOrderCombo);
    m_correctOptionCombo->setMinimumHeight(46);
    m_optionOrderCombo->setMinimumHeight(46);

    QVBoxLayout *optionsFormLayout = nullptr;
    auto *optionsFormCard = createSectionCard("3. Варианты ответа", pageCard, &optionsFormLayout);
    auto *optionsHint = new QLabel(
        "Нужно заполнить четыре варианта, отметить правильный ответ и при необходимости изменить порядок вариантов.",
        optionsFormCard);
    optionsHint->setObjectName("sectionHintLabel");
    optionsHint->setWordWrap(true);

    auto *optionsGrid = new QGridLayout();
    optionsGrid->setHorizontalSpacing(12);
    optionsGrid->setVerticalSpacing(10);
    optionsGrid->addWidget(createFieldLabel("Вариант 1", optionsFormCard), 0, 0);
    optionsGrid->addWidget(createFieldLabel("Вариант 2", optionsFormCard), 0, 1);
    optionsGrid->addWidget(m_optionOneEdit, 1, 0);
    optionsGrid->addWidget(m_optionTwoEdit, 1, 1);
    optionsGrid->addWidget(createFieldLabel("Вариант 3", optionsFormCard), 2, 0);
    optionsGrid->addWidget(createFieldLabel("Вариант 4", optionsFormCard), 2, 1);
    optionsGrid->addWidget(m_optionThreeEdit, 3, 0);
    optionsGrid->addWidget(m_optionFourEdit, 3, 1);

    auto *answerSettingsColumn = new QVBoxLayout();
    answerSettingsColumn->setSpacing(10);
    answerSettingsColumn->addWidget(createFieldLabel("Правильный ответ", optionsFormCard));
    answerSettingsColumn->addWidget(m_correctOptionCombo);
    answerSettingsColumn->addWidget(createFieldLabel("Какой вариант переставить", optionsFormCard));
    answerSettingsColumn->addWidget(m_optionOrderCombo);

    m_moveOptionUpButton = new QPushButton("Поднять вариант выше", questionFormCard);
    m_moveOptionUpButton->setObjectName("cardGhostButton");
    m_moveOptionDownButton = new QPushButton("Опустить вариант ниже", questionFormCard);
    m_moveOptionDownButton->setObjectName("cardGhostButton");

    auto *moveRow = new QHBoxLayout();
    moveRow->setSpacing(10);
    moveRow->addWidget(m_moveOptionUpButton);
    moveRow->addWidget(m_moveOptionDownButton);
    moveRow->addStretch();

    QVBoxLayout *reviewFormLayout = nullptr;
    auto *reviewFormCard = createSectionCard("4. Проверка и сохранение", pageCard, &reviewFormLayout);
    auto *reviewHint = new QLabel(
        "Здесь показано, как вопрос будет выглядеть для студента. После проверки можно сохранить новый вопрос или обновить выбранный.",
        reviewFormCard);
    reviewHint->setObjectName("sectionHintLabel");
    reviewHint->setWordWrap(true);

    m_reviewTitleLabel = new QLabel("Тест не выбран", reviewFormCard);
    m_reviewTitleLabel->setObjectName("courseCardTitleLabel");
    m_reviewTitleLabel->setWordWrap(true);

    m_reviewQuestionLabel = new QLabel("Вопрос пока не заполнен", reviewFormCard);
    m_reviewQuestionLabel->setObjectName("sectionHintLabel");
    m_reviewQuestionLabel->setWordWrap(true);

    m_reviewOptionsLabel = new QLabel("Варианты ответа пока не заполнены", reviewFormCard);
    m_reviewOptionsLabel->setObjectName("sectionHintLabel");
    m_reviewOptionsLabel->setWordWrap(true);

    m_addQuestionButton = new QPushButton("Сохранить новый вопрос", reviewFormCard);
    m_addQuestionButton->setObjectName("cardAccentButton");
    m_loadQuestionButton = new QPushButton("Редактировать выбранный вопрос", reviewFormCard);
    m_loadQuestionButton->setObjectName("cardGhostButton");
    m_updateQuestionButton = new QPushButton("Сохранить изменения вопроса", reviewFormCard);
    m_updateQuestionButton->setObjectName("cardAccentButton");
    m_deleteQuestionButton = new QPushButton("Удалить вопрос", reviewFormCard);
    m_deleteQuestionButton->setObjectName("cardDangerButton");

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

    questionFormLayout->addWidget(questionHint);
    questionFormLayout->addWidget(createFieldLabel("Тест, к которому относится вопрос", questionFormCard));
    questionFormLayout->addWidget(m_questionTestCombo);
    questionFormLayout->addWidget(createFieldLabel("Формулировка вопроса", questionFormCard));
    questionFormLayout->addWidget(m_questionTextEdit);
    questionFormLayout->addStretch();

    optionsFormLayout->addWidget(optionsHint);
    optionsFormLayout->addLayout(optionsGrid);
    optionsFormLayout->addLayout(answerSettingsColumn);
    optionsFormLayout->addLayout(moveRow);
    optionsFormLayout->addStretch();

    reviewFormLayout->addWidget(reviewHint);
    reviewFormLayout->addWidget(m_reviewTitleLabel);
    reviewFormLayout->addWidget(m_reviewQuestionLabel);
    reviewFormLayout->addWidget(m_reviewOptionsLabel);
    reviewFormLayout->addSpacing(10);
    reviewFormLayout->addLayout(questionPrimaryRow);
    reviewFormLayout->addLayout(questionDangerRow);
    reviewFormLayout->addStretch();

    auto *testEditorTab = new QWidget(m_editorTabs);
    auto *testEditorLayout = new QVBoxLayout(testEditorTab);
    testEditorLayout->setContentsMargins(0, 0, 0, 0);
    testEditorLayout->setSpacing(12);
    testEditorLayout->addWidget(testFormCard);
    auto *testNavRow = new QHBoxLayout();
    auto *testNextButton = new QPushButton("Далее: вопрос", testEditorTab);
    testNextButton->setObjectName("cardAccentButton");
    testNavRow->addStretch();
    testNavRow->addWidget(testNextButton);
    testEditorLayout->addLayout(testNavRow);
    testEditorLayout->addStretch();

    auto *questionEditorTab = new QWidget(m_editorTabs);
    auto *questionEditorLayout = new QVBoxLayout(questionEditorTab);
    questionEditorLayout->setContentsMargins(0, 0, 0, 0);
    questionEditorLayout->setSpacing(12);
    questionEditorLayout->addWidget(questionFormCard);
    auto *questionNavRow = new QHBoxLayout();
    auto *questionBackButton = new QPushButton("Назад: тест", questionEditorTab);
    questionBackButton->setObjectName("cardGhostButton");
    auto *questionNextButton = new QPushButton("Далее: варианты", questionEditorTab);
    questionNextButton->setObjectName("cardAccentButton");
    questionNavRow->addWidget(questionBackButton);
    questionNavRow->addStretch();
    questionNavRow->addWidget(questionNextButton);
    questionEditorLayout->addLayout(questionNavRow);
    questionEditorLayout->addStretch();

    auto *optionsEditorTab = new QWidget(m_editorTabs);
    auto *optionsEditorLayout = new QVBoxLayout(optionsEditorTab);
    optionsEditorLayout->setContentsMargins(0, 0, 0, 0);
    optionsEditorLayout->setSpacing(12);
    optionsEditorLayout->addWidget(optionsFormCard);
    auto *optionsNavRow = new QHBoxLayout();
    auto *optionsBackButton = new QPushButton("Назад: вопрос", optionsEditorTab);
    optionsBackButton->setObjectName("cardGhostButton");
    auto *optionsNextButton = new QPushButton("Далее: проверка", optionsEditorTab);
    optionsNextButton->setObjectName("cardAccentButton");
    optionsNavRow->addWidget(optionsBackButton);
    optionsNavRow->addStretch();
    optionsNavRow->addWidget(optionsNextButton);
    optionsEditorLayout->addLayout(optionsNavRow);
    optionsEditorLayout->addStretch();

    auto *reviewEditorTab = new QWidget(m_editorTabs);
    auto *reviewEditorLayout = new QVBoxLayout(reviewEditorTab);
    reviewEditorLayout->setContentsMargins(0, 0, 0, 0);
    reviewEditorLayout->setSpacing(12);
    reviewEditorLayout->addWidget(reviewFormCard);
    auto *reviewNavRow = new QHBoxLayout();
    auto *reviewBackButton = new QPushButton("Назад: варианты", reviewEditorTab);
    reviewBackButton->setObjectName("cardGhostButton");
    reviewNavRow->addWidget(reviewBackButton);
    reviewNavRow->addStretch();
    reviewEditorLayout->addLayout(reviewNavRow);
    reviewEditorLayout->addStretch();

    m_editorTabs->addTab(testEditorTab, "1. Тест");
    m_editorTabs->addTab(questionEditorTab, "2. Вопрос");
    m_editorTabs->addTab(optionsEditorTab, "3. Варианты");
    m_editorTabs->addTab(reviewEditorTab, "4. Проверка");

    connect(testNextButton, &QPushButton::clicked, this, [this]() { m_editorTabs->setCurrentIndex(1); });
    connect(questionBackButton, &QPushButton::clicked, this, [this]() { m_editorTabs->setCurrentIndex(0); });
    connect(questionNextButton, &QPushButton::clicked, this, [this]() { m_editorTabs->setCurrentIndex(2); });
    connect(optionsBackButton, &QPushButton::clicked, this, [this]() { m_editorTabs->setCurrentIndex(1); });
    connect(optionsNextButton, &QPushButton::clicked, this, [this]() { m_editorTabs->setCurrentIndex(3); });
    connect(reviewBackButton, &QPushButton::clicked, this, [this]() { m_editorTabs->setCurrentIndex(2); });

    contentLayout->addLayout(leftColumn, 2);
    contentLayout->addWidget(m_editorTabs, 3);

    layout->addWidget(backButton);
    layout->addWidget(m_courseTitleLabel);
    layout->addWidget(m_courseDescriptionLabel);
    layout->addWidget(m_messageLabel);
    layout->addLayout(summaryLayout);
    layout->addLayout(contentLayout);

    rootLayout->addWidget(pageCard);

    connect(backButton, &QPushButton::clicked, this, &TeacherTestEditorPage::backRequested);
    connect(m_testTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_testMaxAttemptsSpin, qOverload<int>(&QSpinBox::valueChanged), this, [this]() { updateActionState(); });
    connect(m_testTimeLimitSpin, qOverload<int>(&QSpinBox::valueChanged), this, [this]() { updateActionState(); });
    connect(m_testsList, &QListWidget::itemSelectionChanged, this, [this]() {
        updateActionState();

        QListWidgetItem *currentItem = m_testsList->currentItem();
        if (!currentItem) {
            return;
        }

        const QVariant testIdValue = currentItem->data(Qt::UserRole);
        if (!testIdValue.isValid()) {
            return;
        }

        const int testId = testIdValue.toInt();

        for (const auto &test : std::as_const(m_tests)) {
            if (test.id == testId) {
                populateTestDraft(test);
                m_editorTabs->setCurrentIndex(1);

                m_questionTestCombo->blockSignals(true);
                for (int i = 0; i < m_questionTestCombo->count(); ++i) {
                    if (m_questionTestCombo->itemData(i).toInt() == testId) {
                        m_questionTestCombo->setCurrentIndex(i);
                        break;
                    }
                }
                m_questionTestCombo->blockSignals(false);

                emit testSelectedForQuestions(testId);
                showMessage("Тест выбран. Справа можно редактировать его и связанные вопросы.", false);
                return;
            }
        }
    });
    connect(m_questionsList, &QListWidget::itemSelectionChanged, this, [this]() {
        updateActionState();

        const int questionId = selectedQuestionId();
        if (questionId < 0) {
            return;
        }

        for (const auto &question : std::as_const(m_questions)) {
            if (question.id == questionId) {
                populateQuestionDraft(question);
                m_editorTabs->setCurrentIndex(2);
                showMessage("Вопрос выбран. Ниже можно сразу править текст и варианты ответа.", false);
                return;
            }
        }
    });
    connect(m_questionTestCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
        updateActionState();
        if (selectedManagedTestId() >= 0) {
            emit testSelectedForQuestions(selectedManagedTestId());
        }
    });
    connect(m_questionTextEdit, &QTextEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionOneEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionTwoEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionThreeEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionFourEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_correctOptionCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() { updateActionState(); });
    connect(m_optionOrderCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() { updateActionState(); });

    connect(m_addTestButton, &QPushButton::clicked, this, [this]() {
        emit createTestRequested(m_course.id, m_testTitleEdit->text().trimmed(), "active", {}, m_testMaxAttemptsSpin->value(), m_testTimeLimitSpin->value());
    });
    connect(m_loadTestButton, &QPushButton::clicked, this, [this]() {
        const int testId = selectedManagedTestId();
        for (const auto &test : std::as_const(m_tests)) {
            if (test.id == testId) {
                populateTestDraft(test);
                showMessage("Тест загружен в отдельный редактор.", false);
                return;
            }
        }
    });
    connect(m_updateTestButton, &QPushButton::clicked, this, [this]() {
        const TestData currentTest = currentEditingTest();
        emit updateTestRequested(
            m_editingTestId,
            m_testTitleEdit->text().trimmed(),
            currentTest.status.isEmpty() ? "active" : currentTest.status,
            currentTest.deadlineAt,
            m_testMaxAttemptsSpin->value(),
            m_testTimeLimitSpin->value());
    });
    connect(m_deleteTestButton, &QPushButton::clicked, this, [this]() {
        emit deleteTestRequested(m_editingTestId);
    });

    connect(m_addQuestionButton, &QPushButton::clicked, this, [this]() {
        emit createQuestionRequested(
            selectedManagedTestId(),
            m_questionTextEdit->toPlainText().trimmed(),
            questionOptionTexts(),
            m_correctOptionCombo->currentIndex());
    });
    connect(m_loadQuestionButton, &QPushButton::clicked, this, [this]() {
        const int questionId = selectedQuestionId();
        for (const auto &question : std::as_const(m_questions)) {
            if (question.id == questionId) {
                populateQuestionDraft(question);
                showMessage("Вопрос загружен в отдельный редактор.", false);
                return;
            }
        }
    });
    connect(m_updateQuestionButton, &QPushButton::clicked, this, [this]() {
        emit updateQuestionRequested(
            m_editingQuestionId,
            m_questionTextEdit->toPlainText().trimmed(),
            questionOptionTexts(),
            m_correctOptionCombo->currentIndex());
    });
    connect(m_deleteQuestionButton, &QPushButton::clicked, this, [this]() {
        emit deleteQuestionRequested(m_editingQuestionId);
    });

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
        showMessage("Порядок вариантов обновлён. Сохраните вопрос, чтобы зафиксировать порядок.", false);
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
        showMessage("Порядок вариантов обновлён. Сохраните вопрос, чтобы зафиксировать порядок.", false);
    });

    clearEditor();
}

void TeacherTestEditorPage::clearEditor()
{
    m_course = CourseData{};
    m_tests.clear();
    m_questions.clear();
    m_editingTestId = -1;
    m_editingQuestionId = -1;

    m_courseTitleLabel->setText("Редактор тестов");
    m_courseDescriptionLabel->setText("Выберите курс в конструкторе, чтобы открыть отдельный редактор тестов.");
    showMessage("Шаг 1: выбрать курс. Шаг 2: выбрать тест. Шаг 3: редактировать вопросы и варианты ответа.", false);
    m_editorTabs->setCurrentIndex(0);

    clearTestDraft();
    clearQuestionDraft();
    refreshSummary();
    refreshTestsList();
    refreshTestSelector();
    refreshQuestionsList();
    updateActionState();
}

void TeacherTestEditorPage::setCourse(const CourseData &course)
{
    m_course = course;
    m_courseTitleLabel->setText(course.title.isEmpty() ? "Курс без названия" : course.title);
    m_courseDescriptionLabel->setText(
        course.description.isEmpty()
            ? "У курса пока нет описания. Редактор ниже помогает управлять тестами и вопросами."
            : course.description);
    showMessage("Сначала выберите тест слева, затем вопрос. Форма справа заполнится автоматически.", false);
    refreshSummary();
    updateActionState();
}

void TeacherTestEditorPage::setTests(const QVector<TestData> &tests)
{
    m_tests = tests;
    refreshSummary();
    refreshTestSelector();
    refreshTestsList();
    updateActionState();
}

void TeacherTestEditorPage::setQuestions(const QVector<QuestionData> &questions)
{
    m_questions = questions;
    refreshSummary();
    refreshQuestionsList();
    updateActionState();
}

void TeacherTestEditorPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

void TeacherTestEditorPage::clearTestDraft()
{
    m_editingTestId = -1;
    m_testTitleEdit->clear();
    m_testMaxAttemptsSpin->setValue(0);
    m_testTimeLimitSpin->setValue(30);
    updateActionState();
}

void TeacherTestEditorPage::clearQuestionDraft()
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

int TeacherTestEditorPage::currentManagedTestId() const
{
    return selectedManagedTestId();
}

void TeacherTestEditorPage::populateTestDraft(const TestData &test)
{
    m_editingTestId = test.id;
    m_testTitleEdit->setText(test.title);
    m_testMaxAttemptsSpin->setValue(test.maxAttempts);
    m_testTimeLimitSpin->setValue(test.timeLimitMinutes);
    refreshReview();
    updateActionState();
}

void TeacherTestEditorPage::populateQuestionDraft(const QuestionData &question)
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
    refreshReview();
    updateActionState();
}

void TeacherTestEditorPage::refreshSummary()
{
    m_testsSummaryLabel->setText(QString::number(m_tests.size()));
    m_questionsSummaryLabel->setText(QString::number(m_questions.size()));
}

void TeacherTestEditorPage::refreshTestsList()
{
    m_testsList->clear();

    if (m_tests.isEmpty()) {
        appendEditorCard(m_testsList, "Тестов пока нет", "Сначала нужно создать первый тест курса, чтобы потом наполнять его вопросами.");
        return;
    }

    for (const TestData &test : std::as_const(m_tests)) {
        const QString statusText = test.status == "closed"
            ? "закрыт"
            : test.available ? "активен" : "дедлайн истёк";
        const QString deadlineText = test.deadlineAt.isEmpty()
            ? "без дедлайна"
            : QString("дедлайн: %1").arg(test.deadlineAt);
        const QString attemptsText = test.maxAttempts == 0
            ? "попытки: без ограничения"
            : QString("попытки: %1").arg(test.maxAttempts);
        const QString timeText = QString("время: %1 мин.").arg(test.timeLimitMinutes);
        appendEditorCard(
            m_testsList,
            test.title,
            QString("ID теста: %1  •  %2  •  %3  •  %4  •  %5")
                .arg(test.id)
                .arg(statusText, deadlineText, attemptsText, timeText));
        m_testsList->item(m_testsList->count() - 1)->setData(Qt::UserRole, test.id);
    }
}

void TeacherTestEditorPage::refreshQuestionsList()
{
    m_questionsList->clear();

    const int testId = selectedManagedTestId();
    if (testId < 0) {
        appendEditorCard(m_questionsList, "Сначала нужен тест", "После этого справа можно будет создавать и обновлять вопросы.");
        return;
    }

    if (m_questions.isEmpty()) {
        appendEditorCard(m_questionsList, "Вопросов пока нет", "Сначала нужно добавить первый вопрос и четыре варианта ответа.");
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

        appendEditorCard(m_questionsList, question.text, correctText);
        m_questionsList->item(m_questionsList->count() - 1)->setData(Qt::UserRole, question.id);
    }
}

void TeacherTestEditorPage::refreshTestSelector()
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

QStringList TeacherTestEditorPage::questionOptionTexts() const
{
    return {
        m_optionOneEdit->text().trimmed(),
        m_optionTwoEdit->text().trimmed(),
        m_optionThreeEdit->text().trimmed(),
        m_optionFourEdit->text().trimmed()
    };
}

void TeacherTestEditorPage::setQuestionOptionTexts(const QStringList &options)
{
    m_optionOneEdit->setText(options.value(0));
    m_optionTwoEdit->setText(options.value(1));
    m_optionThreeEdit->setText(options.value(2));
    m_optionFourEdit->setText(options.value(3));
}

TestData TeacherTestEditorPage::currentEditingTest() const
{
    for (const TestData &test : std::as_const(m_tests)) {
        if (test.id == m_editingTestId) {
            return test;
        }
    }

    TestData fallback;
    fallback.id = m_editingTestId;
    fallback.status = "active";
    return fallback;
}

void TeacherTestEditorPage::refreshReview()
{
    QString testTitle = "Тест не выбран";
    const int testId = selectedManagedTestId();
    for (const TestData &test : std::as_const(m_tests)) {
        if (test.id == testId) {
            testTitle = QString("Тест: %1").arg(test.title);
            break;
        }
    }

    const QString questionText = m_questionTextEdit->toPlainText().trimmed();
    QStringList optionLines;
    const QStringList options = questionOptionTexts();
    const int correctIndex = m_correctOptionCombo->currentIndex();
    for (int i = 0; i < options.size(); ++i) {
        const QString marker = (i == correctIndex) ? "правильный" : "вариант";
        optionLines << QString("%1. %2 (%3)")
                           .arg(i + 1)
                           .arg(options.value(i).isEmpty() ? "не заполнен" : options.value(i))
                           .arg(marker);
    }

    m_reviewTitleLabel->setText(testTitle);
    m_reviewQuestionLabel->setText(
        questionText.isEmpty()
            ? "Вопрос пока не заполнен"
            : QString("Вопрос: %1").arg(questionText));
    m_reviewOptionsLabel->setText(optionLines.join("\n"));
}

void TeacherTestEditorPage::updateActionState()
{
    const bool hasCourse = m_course.id >= 0;
    const bool hasTests = !m_tests.isEmpty();

    m_addTestButton->setEnabled(hasCourse && !m_testTitleEdit->text().trimmed().isEmpty());
    m_loadTestButton->setEnabled(hasTests && selectedManagedTestId() >= 0);
    m_updateTestButton->setEnabled(hasCourse && m_editingTestId >= 0 && !m_testTitleEdit->text().trimmed().isEmpty());
    m_deleteTestButton->setEnabled(m_editingTestId >= 0);

    const bool questionReady =
        !m_questionTextEdit->toPlainText().trimmed().isEmpty() &&
        !m_optionOneEdit->text().trimmed().isEmpty() &&
        !m_optionTwoEdit->text().trimmed().isEmpty() &&
        !m_optionThreeEdit->text().trimmed().isEmpty() &&
        !m_optionFourEdit->text().trimmed().isEmpty();

    m_addQuestionButton->setEnabled(hasCourse && hasTests && selectedManagedTestId() >= 0 && questionReady);
    m_loadQuestionButton->setEnabled(selectedQuestionId() >= 0);
    m_updateQuestionButton->setEnabled(hasCourse && hasTests && m_editingQuestionId >= 0 && questionReady);
    m_deleteQuestionButton->setEnabled(m_editingQuestionId >= 0);
    m_moveOptionUpButton->setEnabled(m_optionOrderCombo->currentIndex() > 0);
    m_moveOptionDownButton->setEnabled(m_optionOrderCombo->currentIndex() >= 0 && m_optionOrderCombo->currentIndex() < 3);

    const bool hasSelectedTest = selectedManagedTestId() >= 0;
    m_editorTabs->setTabEnabled(1, hasSelectedTest);
    m_editorTabs->setTabEnabled(2, hasSelectedTest);
    m_editorTabs->setTabEnabled(3, hasSelectedTest);
    refreshReview();
}

int TeacherTestEditorPage::selectedManagedTestId() const
{
    if (m_questionTestCombo->count() == 0) {
        return -1;
    }

    return m_questionTestCombo->currentData().toInt();
}

int TeacherTestEditorPage::selectedQuestionId() const
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
