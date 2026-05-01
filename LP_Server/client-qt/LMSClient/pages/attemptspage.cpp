#include "attemptspage.h"

#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

AttemptsPage::AttemptsPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("pageCard");

    auto *pageLayout = new QVBoxLayout(pageCard);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->setSpacing(14);

    m_titleLabel = new QLabel("История попыток", pageCard);
    m_titleLabel->setObjectName("sectionTitleLabel");

    m_summaryLabel = new QLabel(
        "После прохождения тестов здесь появятся баллы, проценты и статус прохождения.",
        pageCard);
    m_summaryLabel->setObjectName("sectionHintLabel");
    m_summaryLabel->setWordWrap(true);

    m_table = new QTableWidget(pageCard);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels(
        {"Test ID", "Score", "Total", "Percent", "Passed"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);

    pageLayout->addWidget(m_titleLabel);
    pageLayout->addWidget(m_summaryLabel);
    pageLayout->addWidget(m_table);

    rootLayout->addWidget(pageCard);
}

void AttemptsPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher") {
        m_titleLabel->setText("Аналитика");
        m_summaryLabel->setText(
            "Здесь появится аналитика преподавателя: активность студентов, результаты тестов и общая статистика по курсам.");
        m_table->setRowCount(0);
    } else {
        m_titleLabel->setText("История попыток");
        m_summaryLabel->setText(
            "После прохождения тестов здесь появятся баллы, проценты и статус прохождения.");
    }
}

void AttemptsPage::setAttempts(const QVector<AttemptData> &attempts)
{
    if (m_role == "Teacher") {
        showPlaceholder(
            "Teacher analytics будет следующим шагом: здесь появятся студенты, результаты и показатели по курсам.");
        return;
    }

    m_table->setRowCount(attempts.size());

    if (attempts.isEmpty()) {
        m_summaryLabel->setText(
            "История пока пуста. После первого отправленного теста здесь появятся результаты.");
        return;
    }

    m_summaryLabel->setText(QString("Всего попыток: %1").arg(attempts.size()));

    for (int row = 0; row < attempts.size(); ++row) {
        const AttemptData &attempt = attempts[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(attempt.testId)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(attempt.score)));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::number(attempt.total)));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::number(attempt.percentage, 'f', 1) + "%"));
        m_table->setItem(row, 4, new QTableWidgetItem(attempt.passed ? "Yes" : "No"));
    }
}

void AttemptsPage::showPlaceholder(const QString &message)
{
    m_summaryLabel->setText(message);
    m_table->setRowCount(0);
}

void AttemptsPage::showError(const QString &error)
{
    m_summaryLabel->setText(error);
    m_table->setRowCount(0);
}
