#include "teacherstudentspage.h"
#include "../ui/uistyles.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QColor>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
QFrame *createStatCard(const QString &title, QLabel **valueLabel, QWidget *parent)
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

TeacherStudentsPage::TeacherStudentsPage(QWidget *parent)
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

    auto *titleLabel = new QLabel("Студенты курса", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Выбор курса преподавателя покажет, кто записан на него и какой у студентов текущий прогресс.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    m_courseCombo = ui_styles::createComboBox(pageCard);
    ui_styles::applyComboBoxStyle(m_courseCombo);

    m_overviewTitleLabel = new QLabel("Сводка по студентам курса", pageCard);
    m_overviewTitleLabel->setObjectName("moduleTitleLabel");

    m_messageLabel = new QLabel("Список студентов появится после выбора курса.", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);

    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);
    statsLayout->addWidget(createStatCard("Студенты", &m_studentsCountLabel, pageCard));
    statsLayout->addWidget(createStatCard("Средний прогресс", &m_averageProgressLabel, pageCard));

    m_focusLabel = new QLabel(pageCard);
    m_focusLabel->setObjectName("sectionHintLabel");
    m_focusLabel->setWordWrap(true);

    m_emptyStateLabel = new QLabel(pageCard);
    m_emptyStateLabel->setObjectName("sectionHintLabel");
    m_emptyStateLabel->setWordWrap(true);

    m_studentsTable = new QTableWidget(pageCard);
    m_studentsTable->setColumnCount(5);
    m_studentsTable->setHorizontalHeaderLabels({"ID", "Логин", "Общий", "Уроки", "Тесты"});
    m_studentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_studentsTable->verticalHeader()->setVisible(false);
    m_studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_studentsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_studentsTable->setFocusPolicy(Qt::StrongFocus);
    m_studentsTable->setAlternatingRowColors(true);
    m_studentsTable->setMinimumHeight(300);
    m_studentsTable->setStyleSheet(
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

    m_attemptsTitleLabel = new QLabel("Попытки выбранного студента", pageCard);
    m_attemptsTitleLabel->setObjectName("moduleTitleLabel");

    m_attemptsSummaryLabel = new QLabel("Выберите студента в таблице выше, чтобы посмотреть его попытки по тестам курса.", pageCard);
    m_attemptsSummaryLabel->setObjectName("sectionHintLabel");
    m_attemptsSummaryLabel->setWordWrap(true);

    m_attemptsEmptyStateLabel = new QLabel(pageCard);
    m_attemptsEmptyStateLabel->setObjectName("sectionHintLabel");
    m_attemptsEmptyStateLabel->setWordWrap(true);

    m_attemptsTable = new QTableWidget(pageCard);
    m_attemptsTable->setColumnCount(4);
    m_attemptsTable->setHorizontalHeaderLabels({"Тест", "Результат", "Процент", "Статус"});
    m_attemptsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_attemptsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_attemptsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_attemptsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_attemptsTable->verticalHeader()->setVisible(false);
    m_attemptsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_attemptsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_attemptsTable->setFocusPolicy(Qt::NoFocus);
    m_attemptsTable->setAlternatingRowColors(true);
    m_attemptsTable->setMinimumHeight(220);
    m_attemptsTable->setStyleSheet(
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
    layout->addWidget(m_focusLabel);
    layout->addWidget(m_emptyStateLabel);
    layout->addWidget(m_studentsTable);
    layout->addWidget(m_attemptsTitleLabel);
    layout->addWidget(m_attemptsSummaryLabel);
    layout->addWidget(m_attemptsEmptyStateLabel);
    layout->addWidget(m_attemptsTable);
    rootLayout->addWidget(pageCard);

    connect(m_courseCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index < 0) {
            return;
        }
        emit courseSelected(m_courseCombo->currentData().toInt());
    });

    connect(m_studentsTable, &QTableWidget::itemSelectionChanged, this, [this]() {
        const auto selectedItems = m_studentsTable->selectedItems();
        if (selectedItems.isEmpty()) {
            return;
        }

        const int row = selectedItems.first()->row();
        QTableWidgetItem *idItem = m_studentsTable->item(row, 0);
        QTableWidgetItem *loginItem = m_studentsTable->item(row, 1);
        if (!idItem || !loginItem) {
            return;
        }

        emit studentSelected(idItem->text().toInt(), loginItem->text());
    });

    clearStudents();
}

void TeacherStudentsPage::setCourses(const QVector<CourseData> &courses)
{
    const int previousId = selectedCourseId();

    m_courseCombo->blockSignals(true);
    m_courseCombo->clear();
    m_studentsTable->setRowCount(0);
    clearStudentAttempts();

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
        showMessage("У преподавателя пока нет курсов. Сначала нужно создать курс.", false);
    }
}

void TeacherStudentsPage::setStudents(const QVector<CourseStudentData> &students)
{
    int totalProgress = 0;
    m_studentsTable->setRowCount(students.size());

    for (int row = 0; row < students.size(); ++row) {
        const CourseStudentData &student = students[row];
        totalProgress += student.progress;

        auto *idItem = new QTableWidgetItem(QString::number(student.id));
        auto *loginItem = new QTableWidgetItem(student.login);
        auto *progressItem = new QTableWidgetItem(QString("%1%").arg(student.progress));
        auto *lessonProgressItem = new QTableWidgetItem(QString("%1%").arg(student.lessonProgress));
        auto *testProgressItem = new QTableWidgetItem(QString("%1%").arg(student.testProgress));

        idItem->setTextAlignment(Qt::AlignCenter);
        progressItem->setTextAlignment(Qt::AlignCenter);
        lessonProgressItem->setTextAlignment(Qt::AlignCenter);
        testProgressItem->setTextAlignment(Qt::AlignCenter);
        progressItem->setForeground(student.progress >= 70 ? QColor("#15803d") : QColor("#334155"));
        lessonProgressItem->setForeground(student.lessonProgress >= 70 ? QColor("#15803d") : QColor("#334155"));
        testProgressItem->setForeground(student.testProgress >= 70 ? QColor("#15803d") : QColor("#334155"));

        m_studentsTable->setItem(row, 0, idItem);
        m_studentsTable->setItem(row, 1, loginItem);
        m_studentsTable->setItem(row, 2, progressItem);
        m_studentsTable->setItem(row, 3, lessonProgressItem);
        m_studentsTable->setItem(row, 4, testProgressItem);
    }

    m_studentsCountLabel->setText(QString::number(students.size()));
    const double averageProgress = students.isEmpty()
        ? 0.0
        : static_cast<double>(totalProgress) / static_cast<double>(students.size());
    m_averageProgressLabel->setText(QString("%1%").arg(QString::number(averageProgress, 'f', 1)));

    if (students.isEmpty()) {
        showMessage("На этот курс пока никто не записан.", false);
        m_focusLabel->setText("Когда появятся первые записи на курс, здесь сразу будет видно, сколько студентов уже начали обучение.");
        m_emptyStateLabel->setText("Пока таблица пустая: у выбранного курса нет записанных студентов.");
        m_emptyStateLabel->show();
        m_studentsTable->hide();
        clearStudentAttempts();
    } else {
        showMessage(QString("Найдено студентов: %1").arg(students.size()), false);
        m_focusLabel->setText(
            QString("Средний прогресс группы сейчас %1%. Колонки \"Уроки\" и \"Тесты\" помогают понять, где именно проседает обучение.")
                .arg(QString::number(averageProgress, 'f', 1)));
        m_emptyStateLabel->hide();
        m_studentsTable->show();
        m_studentsTable->selectRow(0);
    }
}

void TeacherStudentsPage::setStudentAttempts(const QString &studentLogin, const QVector<TeacherStudentAttemptData> &attempts)
{
    m_attemptsTitleLabel->setText(QString("Попытки студента: %1").arg(studentLogin));
    m_attemptsTable->setRowCount(attempts.size());

    for (int row = 0; row < attempts.size(); ++row) {
        const auto &attempt = attempts[row];

        auto *testItem = new QTableWidgetItem(attempt.testTitle);
        auto *scoreItem = new QTableWidgetItem(QString("%1/%2").arg(attempt.score).arg(attempt.total));
        auto *percentageItem = new QTableWidgetItem(QString("%1%").arg(QString::number(attempt.percentage, 'f', 1)));
        auto *statusItem = new QTableWidgetItem(attempt.passed ? "Пройден" : "Не пройден");

        scoreItem->setTextAlignment(Qt::AlignCenter);
        percentageItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setTextAlignment(Qt::AlignCenter);
        statusItem->setForeground(attempt.passed ? QColor("#15803d") : QColor("#b91c1c"));

        m_attemptsTable->setItem(row, 0, testItem);
        m_attemptsTable->setItem(row, 1, scoreItem);
        m_attemptsTable->setItem(row, 2, percentageItem);
        m_attemptsTable->setItem(row, 3, statusItem);
    }

    if (attempts.isEmpty()) {
        m_attemptsSummaryLabel->setText(QString("У студента %1 пока нет попыток по тестам выбранного курса.").arg(studentLogin));
        m_attemptsEmptyStateLabel->setText("Когда студент начнёт проходить тесты, здесь появится хронология его результатов.");
        m_attemptsEmptyStateLabel->show();
        m_attemptsTable->hide();
    } else {
        m_attemptsSummaryLabel->setText(
            QString("Показаны попытки студента %1 по тестам выбранного курса.").arg(studentLogin));
        m_attemptsEmptyStateLabel->hide();
        m_attemptsTable->show();
    }
}

void TeacherStudentsPage::clearStudentAttempts()
{
    m_attemptsTitleLabel->setText("Попытки выбранного студента");
    m_attemptsSummaryLabel->setText("Выберите студента в таблице выше, чтобы посмотреть его попытки по тестам курса.");
    m_attemptsEmptyStateLabel->setText("После выбора студента здесь появится таблица его результатов.");
    m_attemptsEmptyStateLabel->show();
    m_attemptsTable->hide();
    m_attemptsTable->setRowCount(0);
}

void TeacherStudentsPage::clearStudents()
{
    m_studentsCountLabel->setText("0");
    m_averageProgressLabel->setText("0.0%");
    m_focusLabel->setText("Выберите курс преподавателя, чтобы увидеть, кто уже записан и как продвигается группа.");
    m_emptyStateLabel->setText("После выбора курса здесь появится таблица записанных студентов.");
    m_emptyStateLabel->show();
    m_studentsTable->hide();
    m_studentsTable->setRowCount(0);
    clearStudentAttempts();
}

void TeacherStudentsPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

int TeacherStudentsPage::selectedCourseId() const
{
    if (m_courseCombo->count() == 0) {
        return -1;
    }

    return m_courseCombo->currentData().toInt();
}
