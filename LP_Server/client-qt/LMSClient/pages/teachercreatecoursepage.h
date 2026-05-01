#pragma once

#include <QWidget>

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

signals:
    void createCourseRequested(const QString &title, const QString &description);

private:
    void updateButtonState();

    bool m_busy = false;
    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QPushButton *m_createButton;
    QLabel *m_statusLabel;
};
