#include "attemptspage.h"

#include <QFrame>
#include <QColor>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QAbstractItemView>

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

    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);

    auto createStatCard = [pageCard](const QString &title, QLabel **valueLabel) {
        auto *card = new QFrame(pageCard);
        card->setObjectName("moduleCard");
        card->setMinimumHeight(136);
        card->setMaximumHeight(156);
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 18, 18, 18);
        cardLayout->setSpacing(8);

        auto *titleLabel = new QLabel(title, card);
        titleLabel->setObjectName("moduleTitleLabel");

        *valueLabel = new QLabel("0", card);
        (*valueLabel)->setObjectName("courseDetailTitleLabel");

        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(*valueLabel);
        cardLayout->addStretch();
        return card;
    };

    statsLayout->addWidget(createStatCard("Попытки", &m_attemptsStatLabel));
    statsLayout->addWidget(createStatCard("Средний результат", &m_averageStatLabel));
    statsLayout->addWidget(createStatCard("Лучший результат", &m_bestStatLabel));
    statsLayout->addWidget(createStatCard("Успешно", &m_passedStatLabel));

    m_insightLabel = new QLabel(pageCard);
    m_insightLabel->setObjectName("sectionHintLabel");
    m_insightLabel->setWordWrap(true);

    m_emptyStateLabel = new QLabel(pageCard);
    m_emptyStateLabel->setObjectName("sectionHintLabel");
    m_emptyStateLabel->setWordWrap(true);

    m_table = new QTableWidget(pageCard);
    m_table->setColumnCount(7);
    updateTableHeaders();
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(false);
    m_table->setMinimumHeight(360);
    m_table->setStyleSheet(
        "QTableWidget {"
        " background: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 18px;"
        " alternate-background-color: #f8fbff;"
        " gridline-color: transparent;"
        "}"
        "QHeaderView::section {"
        " background: #eef5ff;"
        " color: #334155;"
        " border: none;"
        " border-bottom: 1px solid #dbe4f0;"
        " padding: 12px 10px;"
        " font-weight: 700;"
        "}"
        "QTableWidget::item {"
        " padding: 10px;"
        " border-bottom: 1px solid #eef2f7;"
        "}");

    pageLayout->addWidget(m_titleLabel);
    pageLayout->addWidget(m_summaryLabel);
    pageLayout->addLayout(statsLayout);
    pageLayout->addWidget(m_insightLabel);
    pageLayout->addWidget(m_emptyStateLabel);
    pageLayout->addWidget(m_table);

    rootLayout->addWidget(pageCard);

    showPlaceholder("История пока пуста. После первого отправленного теста здесь появятся результаты.");
}

void AttemptsPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher") {
        m_titleLabel->setText("Аналитика");
        m_summaryLabel->setText(
            "Здесь появится аналитика преподавателя: активность студентов, результаты тестов и общая статистика по курсам.");
        showPlaceholder(
            "Teacher analytics вынесена в отдельную страницу. Здесь student-таблица попыток больше не используется.");
    } else {
        m_titleLabel->setText("История попыток");
        m_summaryLabel->setText(
            "После прохождения тестов здесь появятся баллы, проценты и статус прохождения.");
        showPlaceholder("История пока пуста. После первого отправленного теста здесь появятся результаты.");
    }

    updateTableHeaders();
}

void AttemptsPage::setAttempts(const QVector<AttemptData> &attempts)
{
    if (m_role == "Teacher") {
        showPlaceholder(
            "Teacher analytics будет следующим шагом: здесь появятся студенты, результаты и показатели по курсам.");
        return;
    }

    if (attempts.isEmpty()) {
        showPlaceholder("История пока пуста. После первого отправленного теста здесь появятся результаты.");
        return;
    }

    int totalScore = 0;
    int totalPossible = 0;
    int passedCount = 0;
    double bestPercentage = 0.0;
    QString bestTestTitle;
    QString latestSubmittedAt;
    QString latestTestTitle;
    for (const AttemptData &attempt : attempts) {
        totalScore += attempt.score;
        totalPossible += attempt.total;
        if (attempt.passed) {
            ++passedCount;
        }
        if (attempt.percentage >= bestPercentage) {
            bestPercentage = attempt.percentage;
            bestTestTitle = attempt.testTitle.isEmpty()
                ? QString("Тест #%1").arg(attempt.testId)
                : attempt.testTitle;
        }
        if (attempt.submittedAt >= latestSubmittedAt) {
            latestSubmittedAt = attempt.submittedAt;
            latestTestTitle = attempt.testTitle.isEmpty()
                ? QString("Тест #%1").arg(attempt.testId)
                : attempt.testTitle;
        }
    }

    const double averagePercentage = attempts.isEmpty()
        ? 0.0
        : static_cast<double>(totalScore) * 100.0 / static_cast<double>(totalPossible == 0 ? 1 : totalPossible);

    m_summaryLabel->setText(QString("Всего попыток: %1").arg(attempts.size()));
    m_attemptsStatLabel->setText(QString::number(attempts.size()));
    m_averageStatLabel->setText(QString("%1%").arg(QString::number(averagePercentage, 'f', 1)));
    m_bestStatLabel->setText(QString("%1%").arg(QString::number(bestPercentage, 'f', 1)));
    m_passedStatLabel->setText(QString("%1 из %2").arg(passedCount).arg(attempts.size()));
    m_insightLabel->setText(
        QString("Лучший тест: %1. Последняя активность: %2%3.")
            .arg(bestTestTitle)
            .arg(latestTestTitle)
            .arg(latestSubmittedAt.isEmpty() ? QString() : QString(" (%1)").arg(latestSubmittedAt)));
    m_insightLabel->show();
    m_emptyStateLabel->hide();
    m_table->show();
    m_table->setRowCount(attempts.size());
    m_table->clearContents();

    for (int row = 0; row < attempts.size(); ++row) {
        const AttemptData &attempt = attempts[row];
        const QString testTitle = attempt.testTitle.isEmpty()
            ? QString("Тест #%1").arg(attempt.testId)
            : attempt.testTitle;
        auto *testItem = new QTableWidgetItem(testTitle);
        auto *scoreItem = new QTableWidgetItem(QString::number(attempt.score));
        auto *totalItem = new QTableWidgetItem(QString::number(attempt.total));
        auto *percentItem = new QTableWidgetItem(QString::number(attempt.percentage, 'f', 1) + "%");
        auto *statusItem = new QTableWidgetItem(attempt.passed ? "Пройден" : "Не пройден");
        auto *timeItem = new QTableWidgetItem(attempt.submittedAt.isEmpty() ? "—" : attempt.submittedAt);
        auto *recommendationItem = new QTableWidgetItem(attemptRecommendation(attempt));
        statusItem->setForeground(attempt.passed ? QColor("#15803d") : QColor("#b91c1c"));
        recommendationItem->setForeground(attempt.passed ? QColor("#0f766e") : QColor("#b45309"));

        m_table->setItem(row, 0, testItem);
        m_table->setItem(row, 1, scoreItem);
        m_table->setItem(row, 2, totalItem);
        m_table->setItem(row, 3, percentItem);
        m_table->setItem(row, 4, statusItem);
        m_table->setItem(row, 5, timeItem);
        m_table->setItem(row, 6, recommendationItem);
    }

    m_table->resizeRowsToContents();
}

void AttemptsPage::showPlaceholder(const QString &message)
{
    m_summaryLabel->setText(message);
    m_attemptsStatLabel->setText("0");
    m_averageStatLabel->setText("0.0%");
    m_bestStatLabel->setText("0.0%");
    m_passedStatLabel->setText("0");
    m_insightLabel->setText("Пока нет данных для анализа. После теста здесь появится лучший результат и последняя активность.");
    m_insightLabel->show();
    m_emptyStateLabel->setText(message);
    m_emptyStateLabel->show();
    m_table->hide();
    m_table->setRowCount(0);
}

void AttemptsPage::showError(const QString &error)
{
    showPlaceholder(error);
}

void AttemptsPage::updateTableHeaders()
{
    m_table->setHorizontalHeaderLabels({"Тест", "Баллы", "Всего", "Процент", "Статус", "Когда", "Что дальше"});
}

QString AttemptsPage::attemptRecommendation(const AttemptData &attempt) const
{
    if (attempt.percentage >= 90.0) {
        return "Отлично: можно переходить к следующей теме";
    }
    if (attempt.passed) {
        return "Пройдено: стоит закрепить слабые вопросы";
    }
    if (attempt.percentage >= 50.0) {
        return "Почти: стоит повторить материалы и пройти тест ещё раз";
    }
    return "Нужно повторить уроки перед следующей попыткой";
}
