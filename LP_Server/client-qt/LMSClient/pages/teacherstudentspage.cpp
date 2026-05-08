#include "teacherstudentspage.h"

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

    const QString inputStyle =
        "QComboBox {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " font-size: 14px;"
        "}";

    auto *titleLabel = new QLabel("Студенты курса", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Выбери курс преподавателя и посмотри, кто записан на него и какой у студентов текущий прогресс.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    m_courseCombo = new QComboBox(pageCard);
    m_courseCombo->setStyleSheet(inputStyle);

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
    m_studentsTable->setColumnCount(3);
    m_studentsTable->setHorizontalHeaderLabels({"ID", "Логин", "Прогресс"});
    m_studentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_studentsTable->verticalHeader()->setVisible(false);
    m_studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_studentsTable->setFocusPolicy(Qt::NoFocus);
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

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(m_courseCombo);
    layout->addWidget(m_overviewTitleLabel);
    layout->addWidget(m_messageLabel);
    layout->addLayout(statsLayout);
    layout->addWidget(m_focusLabel);
    layout->addWidget(m_emptyStateLabel);
    layout->addWidget(m_studentsTable);
    rootLayout->addWidget(pageCard);

    connect(m_courseCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (index < 0) {
            return;
        }
        emit courseSelected(m_courseCombo->currentData().toInt());
    });

    clearStudents();
}

void TeacherStudentsPage::setCourses(const QVector<CourseData> &courses)
{
    const int previousId = selectedCourseId();

    m_courseCombo->blockSignals(true);
    m_courseCombo->clear();
    m_studentsTable->setRowCount(0);

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
        showMessage("У преподавателя пока нет курсов. Сначала создай курс.", false);
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

        idItem->setTextAlignment(Qt::AlignCenter);
        progressItem->setTextAlignment(Qt::AlignCenter);
        progressItem->setForeground(student.progress >= 70 ? QColor("#15803d") : QColor("#334155"));

        m_studentsTable->setItem(row, 0, idItem);
        m_studentsTable->setItem(row, 1, loginItem);
        m_studentsTable->setItem(row, 2, progressItem);
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
    } else {
        showMessage(QString("Найдено студентов: %1").arg(students.size()), false);
        m_focusLabel->setText(
            QString("Средний прогресс группы сейчас %1%. Используй эту страницу вместе с аналитикой, чтобы отслеживать активность по курсу.")
                .arg(QString::number(averageProgress, 'f', 1)));
        m_emptyStateLabel->hide();
        m_studentsTable->show();
    }
}

void TeacherStudentsPage::clearStudents()
{
    m_studentsCountLabel->setText("0");
    m_averageProgressLabel->setText("0.0%");
    m_focusLabel->setText("Выбери курс преподавателя, чтобы увидеть, кто уже записан и как продвигается группа.");
    m_emptyStateLabel->setText("После выбора курса здесь появится таблица записанных студентов.");
    m_emptyStateLabel->show();
    m_studentsTable->hide();
    m_studentsTable->setRowCount(0);
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
