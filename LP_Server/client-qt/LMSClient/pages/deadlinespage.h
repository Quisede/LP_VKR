#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"

class QLabel;
class QTableWidget;

class DeadlinesPage : public QWidget
{
    Q_OBJECT

public:
    explicit DeadlinesPage(QWidget *parent = nullptr);

    void setCourses(const QVector<CourseData> &courses);

private:
    QLabel *m_summaryLabel;
    QLabel *m_emptyLabel;
    QTableWidget *m_table;
};
