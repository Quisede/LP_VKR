#pragma once

#include <QWidget>

struct CourseData;

class QLabel;
class QPushButton;

class CourseCard : public QWidget
{
    Q_OBJECT

public:
    explicit CourseCard(
        const CourseData &course,
        const QString &role = "Student",
        QWidget *parent = nullptr);

signals:
    void openRequested(int courseId);
    void enrollRequested(int courseId);
    void builderRequested(int courseId);
    void editRequested(int courseId);
    void deleteRequested(int courseId);

private:
    int m_courseId = -1;
};
