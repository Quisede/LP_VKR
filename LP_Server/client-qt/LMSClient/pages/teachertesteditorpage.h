#pragma once

#include <QWidget>
#include <QStringList>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/questionmodel.h"
#include "../models/testmodel.h"

class QComboBox;
class QLabel;
class QListWidget;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QTextEdit;

class TeacherTestEditorPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherTestEditorPage(QWidget *parent = nullptr);

    void clearEditor();
    void setCourse(const CourseData &course);
    void setTests(const QVector<TestData> &tests);
    void setQuestions(const QVector<QuestionData> &questions);
    void showMessage(const QString &message, bool error = false);
    void clearTestDraft();
    void clearQuestionDraft();
    int currentManagedTestId() const;
    void populateTestDraft(const TestData &test);
    void populateQuestionDraft(const QuestionData &question);

signals:
    void backRequested();
    void testSelectedForQuestions(int testId);
    void createTestRequested(int courseId, const QString &title);
    void updateTestRequested(int testId, const QString &title);
    void deleteTestRequested(int testId);
    void createQuestionRequested(int testId, const QString &text, const QStringList &options, int correctOptionIndex);
    void updateQuestionRequested(int questionId, const QString &text, const QStringList &options, int correctOptionIndex);
    void deleteQuestionRequested(int questionId);

private:
    void refreshSummary();
    void refreshTestsList();
    void refreshQuestionsList();
    void refreshTestSelector();
    void refreshReview();
    QStringList questionOptionTexts() const;
    void setQuestionOptionTexts(const QStringList &options);
    void updateActionState();
    int selectedManagedTestId() const;
    int selectedQuestionId() const;

    CourseData m_course;
    QVector<TestData> m_tests;
    QVector<QuestionData> m_questions;

    QLabel *m_courseTitleLabel;
    QLabel *m_courseDescriptionLabel;
    QLabel *m_messageLabel;
    QLabel *m_testsSummaryLabel;
    QLabel *m_questionsSummaryLabel;
    QTabWidget *m_editorTabs;
    QLabel *m_reviewTitleLabel;
    QLabel *m_reviewQuestionLabel;
    QLabel *m_reviewOptionsLabel;
    QListWidget *m_testsList;
    QListWidget *m_questionsList;
    QLineEdit *m_testTitleEdit;
    QPushButton *m_addTestButton;
    QPushButton *m_loadTestButton;
    QPushButton *m_updateTestButton;
    QPushButton *m_deleteTestButton;
    QComboBox *m_questionTestCombo;
    QTextEdit *m_questionTextEdit;
    QLineEdit *m_optionOneEdit;
    QLineEdit *m_optionTwoEdit;
    QLineEdit *m_optionThreeEdit;
    QLineEdit *m_optionFourEdit;
    QComboBox *m_correctOptionCombo;
    QComboBox *m_optionOrderCombo;
    QPushButton *m_moveOptionUpButton;
    QPushButton *m_moveOptionDownButton;
    QPushButton *m_addQuestionButton;
    QPushButton *m_loadQuestionButton;
    QPushButton *m_updateQuestionButton;
    QPushButton *m_deleteQuestionButton;

    int m_editingTestId = -1;
    int m_editingQuestionId = -1;
};
