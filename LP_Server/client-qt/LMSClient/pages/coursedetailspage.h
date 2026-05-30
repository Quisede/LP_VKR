#pragma once

#include <QWidget>
#include <QVector>

#include "../models/coursemodel.h"
#include "../models/coursestudentmodel.h"
#include "../models/lessonmodel.h"
#include "../models/materialmodel.h"
#include "../models/testmodel.h"

class QLabel;
class QListWidget;
class QPushButton;
class QTabWidget;

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
    void setStudents(const QVector<CourseStudentData> &students);
    void showLoadingState();

signals:
    void backRequested();
    void enrollRequested();
    void testSelected(int testId, const QString &title);
    void materialDownloadRequested(int materialId);
    void materialOpenExternalRequested(int materialId);
    void materialLinkOpenRequested(const QString &url);
    void materialTextPreviewRequested(const QString &title, const QString &content);
    void materialReaderRequested(int materialId);
    void lessonCompletedRequested(int lessonId);
    void lessonPreviewRequested(const QString &title, const QString &content);
    void lessonReaderRequested(int lessonId);

private:
    QString m_role = "Student";
    void setListItems(QListWidget *list, const QVector<QPair<QString, QString>> &items, const QString &emptyTitle, const QString &emptySubtitle);
    void refreshOverview();
    QVector<LessonData> m_lessons;
    QVector<MaterialData> m_materials;
    QVector<MaterialData> m_videos;
    QVector<TestData> m_tests;
    QVector<CourseStudentData> m_students;

    CourseData m_course;
    QLabel *m_titleLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_lessonsSummaryLabel;
    QLabel *m_materialsSummaryLabel;
    QLabel *m_videosSummaryLabel;
    QLabel *m_testsSummaryLabel;
    QLabel *m_overviewHintLabel;
    QLabel *m_progressHintLabel;
    QPushButton *m_primaryActionButton;
    QTabWidget *m_sectionsTabs;
    QListWidget *m_lessonsList;
    QListWidget *m_materialsList;
    QListWidget *m_videosList;
    QListWidget *m_testsList;
    QListWidget *m_studentsList;
    QWidget *m_studentsTab;
};
