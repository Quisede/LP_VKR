#include "testrunnerpage.h"

#include <QButtonGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

TestRunnerPage::TestRunnerPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("testCard");

    auto *pageLayout = new QVBoxLayout(pageCard);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->setSpacing(14);

    auto *backButton = new QPushButton("Назад", pageCard);
    backButton->setObjectName("backToCoursesButton");

    m_titleLabel = new QLabel("Тест не выбран", pageCard);
    m_titleLabel->setObjectName("testSectionTitleLabel");
    m_titleLabel->setStyleSheet("color: #0f172a; font-size: 20px; font-weight: 700;");

    m_progressLabel = new QLabel("Вопросы пока не загружены", pageCard);
    m_progressLabel->setObjectName("testHintLabel");
    m_progressLabel->setStyleSheet("color: #475569; font-size: 14px; font-weight: 600;");

    m_questionLabel = new QLabel("Здесь появится вопрос после загрузки теста", pageCard);
    m_questionLabel->setObjectName("questionLabel");
    m_questionLabel->setWordWrap(true);
    m_questionLabel->setStyleSheet("color: #0f172a; font-size: 18px; font-weight: 600;");

    m_answersContainer = new QWidget(pageCard);
    m_answersContainer->setStyleSheet("background: transparent;");
    m_answersLayout = new QVBoxLayout(m_answersContainer);
    m_answersLayout->setContentsMargins(0, 0, 0, 0);
    m_answersLayout->setSpacing(10);

    m_resultLabel = new QLabel(pageCard);
    m_resultLabel->setObjectName("testHintLabel");
    m_resultLabel->setStyleSheet("color: #2563eb; font-size: 14px; font-weight: 600;");
    m_resultLabel->hide();

    auto *navLayout = new QHBoxLayout();
    navLayout->setSpacing(12);

    m_prevButton = new QPushButton("Назад", pageCard);
    m_prevButton->setObjectName("loadTestButton");
    m_nextButton = new QPushButton("Далее", pageCard);
    m_nextButton->setObjectName("loadTestButton");
    m_submitButton = new QPushButton("Отправить", pageCard);
    m_submitButton->setObjectName("submitTestButton");

    navLayout->addWidget(m_prevButton);
    navLayout->addWidget(m_nextButton);
    navLayout->addStretch();
    navLayout->addWidget(m_submitButton);

    pageLayout->addWidget(backButton);
    pageLayout->addWidget(m_titleLabel);
    pageLayout->addWidget(m_progressLabel);
    pageLayout->addWidget(m_questionLabel);
    pageLayout->addWidget(m_answersContainer);
    pageLayout->addWidget(m_resultLabel);
    pageLayout->addLayout(navLayout);

    rootLayout->addWidget(pageCard);

    connect(backButton, &QPushButton::clicked, this, &TestRunnerPage::backRequested);
    connect(m_prevButton, &QPushButton::clicked, this, [this]() {
        if (m_currentIndex > 0) {
            m_currentIndex -= 1;
            renderCurrentQuestion();
        }
    });
    connect(m_nextButton, &QPushButton::clicked, this, [this]() {
        if (m_currentIndex + 1 < m_questions.size()) {
            m_currentIndex += 1;
            renderCurrentQuestion();
        }
    });
    connect(m_submitButton, &QPushButton::clicked, this, [this]() {
        emit submitRequested(m_test.id, selectedAnswers());
    });
}

void TestRunnerPage::setTest(const TestData &test)
{
    m_test = test;
    m_emptyTitle = test.title.isEmpty() ? "Тест" : test.title;
    m_emptyMessage = "Загружаем вопросы теста...";
    m_titleLabel->setText(test.title.isEmpty() ? "Тест" : test.title);
    m_resultLabel->hide();
}

void TestRunnerPage::setQuestions(const QVector<QuestionData> &questions)
{
    m_questions = questions;
    m_selectedAnswers.clear();
    m_currentIndex = 0;
    renderCurrentQuestion();
}

void TestRunnerPage::showResult(const AttemptData &result)
{
    m_resultLabel->setText(
        QString("Результат: %1/%2, %3%, %4")
            .arg(result.score)
            .arg(result.total)
            .arg(QString::number(result.percentage, 'f', 1))
            .arg(result.passed ? "passed" : "not passed"));
    m_resultLabel->show();
}

void TestRunnerPage::showPlaceholder(const QString &title, const QString &message)
{
    m_questions.clear();
    m_selectedAnswers.clear();
    m_currentIndex = 0;
    m_emptyTitle = title;
    m_emptyMessage = message;
    m_resultLabel->hide();

    renderCurrentQuestion();
}

void TestRunnerPage::renderCurrentQuestion()
{
    while (QLayoutItem *item = m_answersLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    if (m_questions.isEmpty()) {
        m_titleLabel->setText(m_emptyTitle);
        m_progressLabel->setText(m_emptyMessage);
        m_questionLabel->setText(m_emptyMessage);
        m_prevButton->setEnabled(false);
        m_nextButton->setEnabled(false);
        m_submitButton->setEnabled(false);
        return;
    }

    const QuestionData &question = m_questions[m_currentIndex];

    m_resultLabel->hide();
    m_progressLabel->setText(
        QString("Вопрос %1 из %2").arg(m_currentIndex + 1).arg(m_questions.size()));
    m_questionLabel->setText(question.text);

    for (const auto &option : question.options) {
        auto *radio = new QRadioButton(option.text, m_answersContainer);
        radio->setStyleSheet(
            "QRadioButton { color: #0f172a; font-size: 15px; font-weight: 500; "
            "background: transparent; padding: 4px 0; spacing: 10px; } "
            "QRadioButton::indicator { width: 18px; height: 18px; }");
        if (m_selectedAnswers.value(question.id, -1) == option.id) {
            radio->setChecked(true);
        }

        connect(radio, &QRadioButton::toggled, this, [this, question, option](bool checked) {
            if (checked) {
                m_selectedAnswers[question.id] = option.id;
            }
        });

        m_answersLayout->addWidget(radio);
    }

    if (question.options.isEmpty()) {
        auto *emptyLabel = new QLabel(
            "Для этого вопроса сервер не вернул вариантов ответа.",
            m_answersContainer);
        emptyLabel->setObjectName("testHintLabel");
        emptyLabel->setWordWrap(true);
        emptyLabel->setStyleSheet("color: #475569; font-size: 14px;");
        m_answersLayout->addWidget(emptyLabel);
    }

    m_prevButton->setEnabled(m_currentIndex > 0);
    m_nextButton->setEnabled(m_currentIndex + 1 < m_questions.size());
    m_submitButton->setEnabled(!m_questions.isEmpty() && !question.options.isEmpty());
}

QVector<QPair<int, int>> TestRunnerPage::selectedAnswers() const
{
    QVector<QPair<int, int>> answers;
    for (auto it = m_selectedAnswers.constBegin(); it != m_selectedAnswers.constEnd(); ++it) {
        answers.push_back(qMakePair(it.key(), it.value()));
    }
    return answers;
}
