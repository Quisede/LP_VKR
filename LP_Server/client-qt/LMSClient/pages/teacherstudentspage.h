#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/coursestudentmodel.h"

class QComboBox;
class QLabel;
class QTableWidget;

class TeacherStudentsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherStudentsPage(QWidget *parent = nullptr);

    void setCourses(const QVector<CourseData> &courses);
    void setStudents(const QVector<CourseStudentData> &students);
    void clearStudents();
    void showMessage(const QString &message, bool error = false);
    int selectedCourseId() const;

signals:
    void courseSelected(int courseId);

private:
    QComboBox *m_courseCombo;
    QLabel *m_messageLabel;
    QTableWidget *m_studentsTable;
};
