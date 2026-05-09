#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/coursestudentmodel.h"
#include "../models/teacherstudentattemptmodel.h"

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
    void setStudentAttempts(const QString &studentLogin, const QVector<TeacherStudentAttemptData> &attempts);
    void clearStudentAttempts();
    void clearStudents();
    void showMessage(const QString &message, bool error = false);
    int selectedCourseId() const;

signals:
    void courseSelected(int courseId);
    void studentSelected(int studentId, const QString &studentLogin);

private:
    QLabel *m_overviewTitleLabel;
    QComboBox *m_courseCombo;
    QLabel *m_messageLabel;
    QLabel *m_studentsCountLabel;
    QLabel *m_averageProgressLabel;
    QLabel *m_focusLabel;
    QLabel *m_emptyStateLabel;
    QLabel *m_attemptsTitleLabel;
    QLabel *m_attemptsSummaryLabel;
    QLabel *m_attemptsEmptyStateLabel;
    QTableWidget *m_studentsTable;
    QTableWidget *m_attemptsTable;
};
