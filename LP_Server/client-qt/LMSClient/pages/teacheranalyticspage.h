#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/teacheranalyticsmodel.h"

class QComboBox;
class QLabel;
class QTableWidget;
class QFrame;

class TeacherAnalyticsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherAnalyticsPage(QWidget *parent = nullptr);

    void setCourses(const QVector<CourseData> &courses);
    void setAnalytics(const TeacherCourseAnalyticsData &analytics);
    void clearAnalytics();
    void showMessage(const QString &message, bool error = false);
    int selectedCourseId() const;

signals:
    void courseSelected(int courseId);

private:
    QLabel *m_overviewTitleLabel;
    QComboBox *m_courseCombo;
    QLabel *m_messageLabel;
    QLabel *m_studentsValueLabel;
    QLabel *m_attemptsValueLabel;
    QLabel *m_averageValueLabel;
    QLabel *m_insightLabel;
    QLabel *m_emptyStateLabel;
    QTableWidget *m_resultsTable;
};
