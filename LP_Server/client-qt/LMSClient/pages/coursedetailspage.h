#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/lessonmodel.h"
#include "../models/materialmodel.h"
#include "../models/testmodel.h"

class QLabel;
class QListWidget;
class QPushButton;

class CourseDetailsPage : public QWidget
{
    Q_OBJECT

public:
    explicit CourseDetailsPage(QWidget *parent = nullptr);

    void setRoleMode(const QString &role);
    void setCourse(const CourseData &course);
    void setLessons(const QVector<LessonData> &lessons);
    void setMaterials(const QVector<MaterialData> &materials, const QVector<MaterialData> &videos);
    void setTests(const QVector<TestData> &tests);
    void showLoadingState();

signals:
    void backRequested();
    void enrollRequested();
    void testSelected(int testId, const QString &title);

private:
    QString m_role = "Student";
    void setListItems(QListWidget *list, const QVector<QPair<QString, QString>> &items, const QString &emptyTitle, const QString &emptySubtitle);

    CourseData m_course;
    QLabel *m_titleLabel;
    QLabel *m_descriptionLabel;
    QPushButton *m_primaryActionButton;
    QListWidget *m_lessonsList;
    QListWidget *m_materialsList;
    QListWidget *m_videosList;
    QListWidget *m_testsList;
};
