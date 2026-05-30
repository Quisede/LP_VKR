#include "deadlinespage.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <algorithm>

DeadlinesPage::DeadlinesPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("pageCard");
    auto *layout = new QVBoxLayout(pageCard);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto *titleLabel = new QLabel("Предстоящие дедлайны", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Здесь собраны ближайшие сроки по активным тестам из доступных курсов.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    m_summaryLabel = new QLabel(pageCard);
    m_summaryLabel->setObjectName("sectionHintLabel");
    m_summaryLabel->setWordWrap(true);

    m_emptyLabel = new QLabel(pageCard);
    m_emptyLabel->setObjectName("sectionHintLabel");
    m_emptyLabel->setWordWrap(true);

    m_table = new QTableWidget(pageCard);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Курс", "Дедлайн", "Прогресс", "Рекомендация"});
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setFocusPolicy(Qt::NoFocus);
    m_table->setAlternatingRowColors(true);
    m_table->setMinimumHeight(420);
    m_table->setStyleSheet(
        "QTableWidget { background-color: #ffffff; alternate-background-color: #f8fbff; color: #0f172a; border: 1px solid #dbe4f0; border-radius: 16px; gridline-color: #e2e8f0; }"
        "QHeaderView::section { background-color: #eff6ff; color: #1e293b; border: none; border-bottom: 1px solid #dbe4f0; padding: 10px 12px; font-weight: 700; }");

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(m_summaryLabel);
    layout->addWidget(m_emptyLabel);
    layout->addWidget(m_table);
    rootLayout->addWidget(pageCard);

    setCourses({});
}

void DeadlinesPage::setCourses(const QVector<CourseData> &courses)
{
    QVector<CourseData> deadlineCourses;
    for (const CourseData &course : courses) {
        if (!course.nearestDeadlineAt.trimmed().isEmpty()) {
            deadlineCourses.push_back(course);
        }
    }

    std::sort(deadlineCourses.begin(), deadlineCourses.end(), [](const CourseData &left, const CourseData &right) {
        return left.nearestDeadlineAt < right.nearestDeadlineAt;
    });

    m_table->clearContents();
    m_table->setRowCount(deadlineCourses.size());

    for (int row = 0; row < deadlineCourses.size(); ++row) {
        const CourseData &course = deadlineCourses[row];
        const QString progress = QString("%1 из %2 тестов пройдено")
            .arg(course.passedTestsCount)
            .arg(course.testsCount);
        const QString recommendation = course.passedTestsCount >= course.testsCount && course.testsCount > 0
            ? "Курс закрыт по тестам, можно улучшить результат."
            : "Откройте курс и пройдите ближайший активный тест.";

        m_table->setItem(row, 0, new QTableWidgetItem(course.title));
        m_table->setItem(row, 1, new QTableWidgetItem(course.nearestDeadlineAt));
        m_table->setItem(row, 2, new QTableWidgetItem(progress));
        m_table->setItem(row, 3, new QTableWidgetItem(recommendation));
    }

    m_table->resizeRowsToContents();
    m_summaryLabel->setText(deadlineCourses.isEmpty()
        ? "Активных дедлайнов сейчас нет."
        : QString("Найдено ближайших дедлайнов: %1. Первый в списке требует внимания раньше остальных.")
            .arg(deadlineCourses.size()));
    m_emptyLabel->setVisible(deadlineCourses.isEmpty());
    m_emptyLabel->setText("Когда преподаватель задаст дедлайн для активного теста, он появится здесь.");
    m_table->setVisible(!deadlineCourses.isEmpty());
}
