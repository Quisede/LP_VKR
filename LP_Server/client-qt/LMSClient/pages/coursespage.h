#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"

class QListWidget;
class QLabel;
class QLineEdit;
class QVBoxLayout;
class QWidget;

class CoursesPage : public QWidget
{
    Q_OBJECT

public:
    explicit CoursesPage(QWidget *parent = nullptr);

    void setRoleMode(const QString &role);
    void setCourses(const QVector<CourseData> &courses);
    void showPlaceholder(const QString &title, const QString &message);

signals:
    void courseOpened(const CourseData &course);
    void enrollRequested(int courseId);
    void courseBuilderRequested(const CourseData &course);
    void courseEditRequested(const CourseData &course);
    void courseDeleteRequested(const CourseData &course);

private:
    void applyCourseFilter(const QString &query = QString());
    void clearCards();
    void addCardWidget(QWidget *widget);

    QString m_role = "Student";
    int m_coursesCount = 0;
    QVector<CourseData> m_allCourses;
    QWidget *m_cardsContainer;
    QVBoxLayout *m_cardsLayout;
    QLabel *m_titleLabel;
    QLabel *m_hintLabel;
    QLineEdit *m_searchEdit;
    QLabel *m_countValueLabel;
    QLabel *m_countTitleLabel;
    QLabel *m_focusValueLabel;
    QLabel *m_focusTitleLabel;
    QLabel *m_infoLabel;
};
