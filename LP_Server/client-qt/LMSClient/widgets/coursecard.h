#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

class CourseCard : public QWidget
{
    Q_OBJECT

public:
    explicit CourseCard(
        int courseId,
        const QString &title,
        const QString &description,
        bool showEnrollAction = true,
        QWidget *parent = nullptr);

signals:
    void openRequested(int courseId);
    void enrollRequested(int courseId);
    void builderRequested(int courseId);

private:
    int m_courseId = -1;
};
