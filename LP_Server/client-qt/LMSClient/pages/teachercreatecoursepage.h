#pragma once

#include <QWidget>

struct CourseData;

class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class TeacherCreateCoursePage : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherCreateCoursePage(QWidget *parent = nullptr);

    void clearForm();
    void setBusy(bool busy);
    void showMessage(const QString &message, bool error = false);
    void setCreateMode();
    void setEditMode(const CourseData &course);
    bool isEditMode() const;
    int editingCourseId() const;

signals:
    void createCourseRequested(const QString &title, const QString &description);
    void updateCourseRequested(int courseId, const QString &title, const QString &description);
    void deleteCourseRequested(int courseId);

private:
    void updateButtonState();

    bool m_busy = false;
    bool m_editMode = false;
    int m_editingCourseId = -1;
    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QPushButton *m_submitButton;
    QPushButton *m_deleteButton;
    QLabel *m_titleLabel;
    QLabel *m_hintLabel;
    QLabel *m_statusLabel;
};
