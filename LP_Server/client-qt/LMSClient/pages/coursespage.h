#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"

class QListWidget;
class QLabel;
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

private:
    void clearCards();
    void addCardWidget(QWidget *widget);

    QString m_role = "Student";
    QWidget *m_cardsContainer;
    QVBoxLayout *m_cardsLayout;
    QLabel *m_titleLabel;
    QLabel *m_hintLabel;
};
