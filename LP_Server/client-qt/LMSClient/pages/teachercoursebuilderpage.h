#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/lessonmodel.h"
#include "../models/materialmodel.h"
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
    void showMessage(const QString &message, bool error = false);
    void clearLessonDraft();
    void clearMaterialDraft();
    void clearTestDraft();

signals:
    void createLessonRequested(int courseId, const QString &title, const QString &content);
    void createMaterialRequested(int lessonId, const QString &title, const QString &type, const QString &content);
    void createTestRequested(int courseId, const QString &title);

private:
    void refreshOverview();
    void refreshLessonsList();
    void refreshLessonSelector();
    void refreshMaterialsList();
    void refreshTestsList();
    int selectedLessonId() const;
    void updateActionState();

    CourseData m_course;
    QVector<LessonData> m_lessons;
    QVector<MaterialData> m_materials;
    QVector<TestData> m_tests;

    QLabel *m_courseTitleLabel;
    QLabel *m_courseDescriptionLabel;
    QLabel *m_messageLabel;
    QLabel *m_lessonsSummaryLabel;
    QLabel *m_materialsSummaryLabel;
    QLabel *m_testsSummaryLabel;

    QListWidget *m_lessonsList;
    QListWidget *m_materialsList;
    QListWidget *m_testsList;

    QLineEdit *m_lessonTitleEdit;
    QTextEdit *m_lessonContentEdit;
    QPushButton *m_addLessonButton;

    QComboBox *m_materialLessonCombo;
    QLineEdit *m_materialTitleEdit;
    QComboBox *m_materialTypeCombo;
    QTextEdit *m_materialContentEdit;
    QPushButton *m_addMaterialButton;

    QLineEdit *m_testTitleEdit;
    QPushButton *m_addTestButton;
};
