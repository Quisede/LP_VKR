#pragma once

#include <QString>
#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/teacheranalyticsmodel.h"

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QFrame;

class TeacherAnalyticsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherAnalyticsPage(QWidget *parent = nullptr);

    void setRoleMode(const QString &role);
    void setCourses(const QVector<CourseData> &courses);
    void setAnalytics(const TeacherCourseAnalyticsData &analytics);
    void clearAnalytics();
    void showMessage(const QString &message, bool error = false);
    int selectedCourseId() const;

signals:
    void courseSelected(int courseId);
    void exportCsvRequested();
    void exportPdfRequested();

private:
    QString m_role = "Teacher";
    QLabel *m_overviewTitleLabel;
    QComboBox *m_courseCombo;
    QLabel *m_messageLabel;
    QLabel *m_studentsValueLabel;
    QLabel *m_attemptsValueLabel;
    QLabel *m_averageValueLabel;
    QLabel *m_lessonAverageValueLabel;
    QLabel *m_insightLabel;
    QLabel *m_emptyStateLabel;
    QTableWidget *m_resultsTable;
    QPushButton *m_exportCsvButton;
    QPushButton *m_exportPdfButton;
};
