#include "teacheranalyticspage.h"
#include "../ui/uistyles.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace {

QFrame *createStatCard(
    const QString &title,
    QLabel **valueLabel,
    QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("moduleCard");
    card->setMinimumHeight(136);
    card->setMaximumHeight(156);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("moduleTitleLabel");

    *valueLabel = new QLabel("0", card);
    (*valueLabel)->setObjectName("courseDetailTitleLabel");

    layout->addWidget(titleLabel);
    layout->addWidget(*valueLabel);
    layout->addStretch();
    return card;
}

}

TeacherAnalyticsPage::TeacherAnalyticsPage(QWidget *parent)
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

    auto *titleLabel = new QLabel("Аналитика курса", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Выбор курса преподавателя покажет количество студентов, попыток и сводную таблицу результатов по тестам.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    m_courseCombo = ui_styles::createComboBox(pageCard);
    ui_styles::applyComboBoxStyle(m_courseCombo);

    m_overviewTitleLabel = new QLabel("Сводка по выбранному курсу", pageCard);
    m_overviewTitleLabel->setObjectName("moduleTitleLabel");

    m_messageLabel = new QLabel("Выберите курс, чтобы загрузить аналитику.", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);

    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);
    statsLayout->addWidget(createStatCard("Студенты", &m_studentsValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("Попытки", &m_attemptsValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("Уроки изучены", &m_lessonAverageValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("Средний балл", &m_averageValueLabel, pageCard));

    m_insightLabel = new QLabel(pageCard);
    m_insightLabel->setObjectName("sectionHintLabel");
    m_insightLabel->setWordWrap(true);

    m_emptyStateLabel = new QLabel(pageCard);
    m_emptyStateLabel->setObjectName("sectionHintLabel");
    m_emptyStateLabel->setWordWrap(true);

    m_resultsTable = new QTableWidget(pageCard);
    m_resultsTable->setColumnCount(5);
    m_resultsTable->setHorizontalHeaderLabels({"Студент", "Тест", "Результат", "Процент", "Статус"});
    m_resultsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_resultsTable->verticalHeader()->setVisible(false);
    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_resultsTable->setFocusPolicy(Qt::NoFocus);
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setMinimumHeight(320);
    m_resultsTable->setStyleSheet(
        "QTableWidget {"
        " background-color: #ffffff;"
        " alternate-background-color: #f8fbff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 16px;"
        " gridline-color: #e2e8f0;"
        "}"
        "QHeaderView::section {"
        " background-color: #eff6ff;"
        " color: #1e293b;"
        " border: none;"
        " border-bottom: 1px solid #dbe4f0;"
        " padding: 10px 12px;"
        " font-weight: 700;"
        "}");

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(m_courseCombo);
    layout->addWidget(m_overviewTitleLabel);
    layout->addWidget(m_messageLabel);
    layout->addLayout(statsLayout);
    layout->addWidget(m_insightLabel);
    layout->addWidget(m_emptyStateLabel);
    layout->addWidget(m_resultsTable);
    rootLayout->addWidget(pageCard);

    connect(m_courseCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index < 0) {
            return;
        }

        emit courseSelected(m_courseCombo->currentData().toInt());
    });

    clearAnalytics();
}

void TeacherAnalyticsPage::setRoleMode(const QString &role)
{
    m_role = role;
}

void TeacherAnalyticsPage::setCourses(const QVector<CourseData> &courses)
{
    const int previousId = selectedCourseId();

    m_courseCombo->blockSignals(true);
    m_courseCombo->clear();
    clearAnalytics();

    for (const CourseData &course : courses) {
        m_courseCombo->addItem(course.title, course.id);
    }

    if (!courses.isEmpty()) {
        int selectedIndex = 0;
        for (int i = 0; i < m_courseCombo->count(); ++i) {
            if (m_courseCombo->itemData(i).toInt() == previousId) {
                selectedIndex = i;
                break;
            }
        }
        m_courseCombo->setCurrentIndex(selectedIndex);
    }

    m_courseCombo->blockSignals(false);

    if (m_courseCombo->count() > 0) {
        emit courseSelected(selectedCourseId());
    } else {
        showMessage(
            m_role == "Admin"
                ? "В системе пока нет курсов. Как только они появятся, администратор увидит их здесь."
                : "У преподавателя пока нет курсов. Сначала нужно создать курс.",
            false);
    }
}

void TeacherAnalyticsPage::setAnalytics(const TeacherCourseAnalyticsData &analytics)
{
    m_studentsValueLabel->setText(QString::number(analytics.studentsCount));
    m_attemptsValueLabel->setText(QString::number(analytics.attemptsCount));
    m_lessonAverageValueLabel->setText(QString("%1%").arg(QString::number(analytics.averageLessonProgress, 'f', 1)));
    m_averageValueLabel->setText(QString("%1%").arg(QString::number(analytics.averagePercentage, 'f', 1)));
    m_resultsTable->show();
    m_emptyStateLabel->hide();

    m_resultsTable->setRowCount(analytics.rows.size());

    for (int row = 0; row < analytics.rows.size(); ++row) {
        const TeacherAnalyticsRowData &result = analytics.rows[row];

        auto *studentItem = new QTableWidgetItem(result.studentLogin);
        auto *testItem = new QTableWidgetItem(result.testTitle);
        auto *scoreItem = new QTableWidgetItem(QString("%1/%2").arg(result.score).arg(result.total));
        auto *percentageItem = new QTableWidgetItem(QString("%1%").arg(QString::number(result.percentage, 'f', 1)));
        auto *statusItem = new QTableWidgetItem(result.passed ? "Пройден" : "Не пройден");

        scoreItem->setTextAlignment(Qt::AlignCenter);
        percentageItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setForeground(result.passed ? QColor("#15803d") : QColor("#b91c1c"));

        m_resultsTable->setItem(row, 0, studentItem);
        m_resultsTable->setItem(row, 1, testItem);
        m_resultsTable->setItem(row, 2, scoreItem);
        m_resultsTable->setItem(row, 3, percentageItem);
        m_resultsTable->setItem(row, 4, statusItem);
    }

    if (analytics.rows.isEmpty()) {
        m_insightLabel->setText("Пока ещё рано для глубокой сводки: у курса нет попыток прохождения тестов.");
        showMessage("Для этого курса пока нет попыток прохождения тестов.", false);
        m_emptyStateLabel->setText("Когда студенты начнут проходить тесты, здесь появится подробная таблица результатов по каждому тесту.");
        m_emptyStateLabel->show();
        m_resultsTable->hide();
    } else {
        m_insightLabel->setText(
            QString("На курсе уже есть %1 попыток. Уроки в среднем изучены на %2%, средний результат тестов — %3%.")
                .arg(analytics.attemptsCount)
                .arg(QString::number(analytics.averageLessonProgress, 'f', 1))
                .arg(QString::number(analytics.averagePercentage, 'f', 1)));
        showMessage(QString("Найдено попыток: %1").arg(analytics.rows.size()), false);
    }
}

void TeacherAnalyticsPage::clearAnalytics()
{
    m_studentsValueLabel->setText("0");
    m_attemptsValueLabel->setText("0");
    m_lessonAverageValueLabel->setText("0.0%");
    m_averageValueLabel->setText("0.0%");
    m_insightLabel->setText("Выберите курс, чтобы увидеть число студентов, попыток и общую картину по тестам.");
    m_emptyStateLabel->setText("После выбора курса здесь появится подробная таблица результатов.");
    m_emptyStateLabel->show();
    m_resultsTable->hide();
    m_resultsTable->setRowCount(0);
}

void TeacherAnalyticsPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

int TeacherAnalyticsPage::selectedCourseId() const
{
    if (m_courseCombo->count() == 0) {
        return -1;
    }

    return m_courseCombo->currentData().toInt();
}
