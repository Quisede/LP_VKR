#pragma once

#include <QMap>
#include <QPair>
#include <QWidget>
#include <QVector>

#include "../models/attemptmodel.h"
#include "../models/questionmodel.h"
#include "../models/testmodel.h"

class QLabel;
class QPushButton;
class QVBoxLayout;
class QButtonGroup;
class QFrame;

class TestRunnerPage : public QWidget
{
    Q_OBJECT

public:
    explicit TestRunnerPage(QWidget *parent = nullptr);

    void setTest(const TestData &test);
    void setQuestions(const QVector<QuestionData> &questions);
    void showResult(const AttemptData &result);
    void showPlaceholder(const QString &title, const QString &message);

signals:
    void backRequested();
    void submitRequested(int testId, const QVector<QPair<int, int>> &answers);

private:
    void renderCurrentQuestion();
    QVector<QPair<int, int>> selectedAnswers() const;

    TestData m_test;
    QVector<QuestionData> m_questions;
    QMap<int, int> m_selectedAnswers;
    int m_currentIndex = 0;
    QString m_emptyTitle = "Тест не выбран";
    QString m_emptyMessage = "Выбери тест внутри курса, чтобы увидеть вопрос и варианты ответов.";

    QLabel *m_titleLabel;
    QLabel *m_progressLabel;
    QLabel *m_questionLabel;
    QLabel *m_resultLabel;
    QLabel *m_hintLabel;
    QLabel *m_questionNumberValueLabel;
    QLabel *m_totalQuestionsValueLabel;
    QLabel *m_answersCountValueLabel;
    QWidget *m_answersContainer;
    QVBoxLayout *m_answersLayout;
    QPushButton *m_prevButton;
    QPushButton *m_nextButton;
    QPushButton *m_submitButton;
};
