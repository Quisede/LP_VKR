#pragma once

#include <QWidget>
#include <QStringList>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/lessonmodel.h"
#include "../models/materialmodel.h"
#include "../models/questionmodel.h"
#include "../models/testmodel.h"

class QComboBox;
class QLabel;
class QListWidget;
class QLineEdit;
class QPushButton;
class QTextEdit;

class TeacherCourseBuilderPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherCourseBuilderPage(QWidget *parent = nullptr);

    void clearBuilder();
    void setCourse(const CourseData &course);
    void setLessons(const QVector<LessonData> &lessons);
    void setMaterials(const QVector<MaterialData> &materials);
    void setTests(const QVector<TestData> &tests);
    void setQuestions(const QVector<QuestionData> &questions);
    void showMessage(const QString &message, bool error = false);
    void clearLessonDraft();
    void clearMaterialDraft();
    void clearTestDraft();
    void clearQuestionDraft();
    int currentManagedTestId() const;
    void populateTestDraft(const TestData &test);
    void populateQuestionDraft(const QuestionData &question);
    void populateLessonDraft(const LessonData &lesson);
    void populateMaterialDraft(const MaterialData &material);

signals:
    void createLessonRequested(int courseId, const QString &title, const QString &content);
    void updateLessonRequested(int lessonId, const QString &title, const QString &content);
    void deleteLessonRequested(int lessonId);
    void createMaterialRequested(int lessonId, const QString &title, const QString &type, const QString &content);
    void updateMaterialRequested(int materialId, const QString &title, const QString &type, const QString &content);
    void deleteMaterialRequested(int materialId);
    void createTestRequested(int courseId, const QString &title);
    void updateTestRequested(int testId, const QString &title);
    void deleteTestRequested(int testId);
    void testSelectedForQuestions(int testId);
    void createQuestionRequested(int testId, const QString &text, const QStringList &options, int correctOptionIndex);
    void updateQuestionRequested(int questionId, const QString &text, const QStringList &options, int correctOptionIndex);
    void deleteQuestionRequested(int questionId);

private:
    void refreshOverview();
    void refreshLessonsList();
    void refreshLessonSelector();
    void refreshMaterialsList();
    void refreshTestSelector();
    void refreshTestsList();
    void refreshQuestionsList();
    int selectedLessonId() const;
    int selectedLessonListId() const;
    int selectedMaterialId() const;
    int selectedManagedTestId() const;
    int selectedQuestionId() const;
    void updateActionState();

    CourseData m_course;
    QVector<LessonData> m_lessons;
    QVector<MaterialData> m_materials;
    QVector<TestData> m_tests;
    QVector<QuestionData> m_questions;

    QLabel *m_courseTitleLabel;
    QLabel *m_courseDescriptionLabel;
    QLabel *m_messageLabel;
    QLabel *m_lessonsSummaryLabel;
    QLabel *m_materialsSummaryLabel;
    QLabel *m_testsSummaryLabel;

    QListWidget *m_lessonsList;
    QListWidget *m_materialsList;
    QListWidget *m_testsList;
    QListWidget *m_questionsList;

    QLineEdit *m_lessonTitleEdit;
    QTextEdit *m_lessonContentEdit;
    QPushButton *m_addLessonButton;
    QPushButton *m_loadLessonButton;
    QPushButton *m_updateLessonButton;
    QPushButton *m_deleteLessonButton;

    QComboBox *m_materialLessonCombo;
    QLineEdit *m_materialTitleEdit;
    QComboBox *m_materialTypeCombo;
    QTextEdit *m_materialContentEdit;
    QPushButton *m_addMaterialButton;
    QPushButton *m_loadMaterialButton;
    QPushButton *m_updateMaterialButton;
    QPushButton *m_deleteMaterialButton;

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
    QPushButton *m_addQuestionButton;
    QPushButton *m_loadQuestionButton;
    QPushButton *m_updateQuestionButton;
    QPushButton *m_deleteQuestionButton;

    int m_editingLessonId = -1;
    int m_editingMaterialId = -1;
    int m_editingTestId = -1;
    int m_editingQuestionId = -1;
};
