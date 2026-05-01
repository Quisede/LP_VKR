#include "dashboardpage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

namespace {
QFrame *createSummaryCard(
    const QString &title,
    QLabel **titleLabelOut,
    QLabel **valueLabel,
    QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("moduleCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("moduleTitleLabel");
    *titleLabelOut = titleLabel;

    *valueLabel = new QLabel("0", card);
    (*valueLabel)->setObjectName("courseDetailTitleLabel");

    layout->addWidget(titleLabel);
    layout->addWidget(*valueLabel);
    layout->addStretch();

    return card;
}
}

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("pageCard");

    auto *pageLayout = new QVBoxLayout(pageCard);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->setSpacing(16);

    m_welcomeLabel = new QLabel("Добро пожаловать", pageCard);
    m_welcomeLabel->setObjectName("sectionTitleLabel");

    m_hintLabel = new QLabel(
        "Это стартовый экран платформы. Здесь можно быстро увидеть, что доступно пользователю прямо сейчас.",
        pageCard);
    m_hintLabel->setObjectName("sectionHintLabel");
    m_hintLabel->setWordWrap(true);

    auto *summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(14);
    summaryLayout->addWidget(createSummaryCard("Курсы", &m_coursesTitleLabel, &m_coursesValueLabel, pageCard));
    summaryLayout->addWidget(createSummaryCard("Тесты", &m_testsTitleLabel, &m_testsValueLabel, pageCard));
    summaryLayout->addWidget(createSummaryCard("Попытки", &m_attemptsTitleLabel, &m_attemptsValueLabel, pageCard));

    m_recentCoursesTitleLabel = new QLabel("Последние доступные курсы", pageCard);
    m_recentCoursesTitleLabel->setObjectName("moduleTitleLabel");

    m_recentCoursesList = new QListWidget(pageCard);
    m_recentCoursesList->setSpacing(10);
    m_recentCoursesList->setSelectionMode(QAbstractItemView::NoSelection);
    m_recentCoursesList->setFocusPolicy(Qt::NoFocus);

    pageLayout->addWidget(m_welcomeLabel);
    pageLayout->addWidget(m_hintLabel);
    pageLayout->addLayout(summaryLayout);
    pageLayout->addWidget(m_recentCoursesTitleLabel);
    pageLayout->addWidget(m_recentCoursesList);

    rootLayout->addWidget(pageCard);
}

void DashboardPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher") {
        m_hintLabel->setText(
            "Это рабочий кабинет преподавателя. Здесь можно видеть свои курсы и готовить основу для управления контентом.");
        m_coursesTitleLabel->setText("Мои курсы");
        m_testsTitleLabel->setText("Тесты");
        m_attemptsTitleLabel->setText("Аналитика");
        m_recentCoursesTitleLabel->setText("Последние курсы преподавателя");
    } else {
        m_hintLabel->setText(
            "Это стартовый экран платформы. Здесь можно быстро увидеть, что доступно пользователю прямо сейчас.");
        m_coursesTitleLabel->setText("Курсы");
        m_testsTitleLabel->setText("Тесты");
        m_attemptsTitleLabel->setText("Попытки");
        m_recentCoursesTitleLabel->setText("Последние доступные курсы");
    }
}

void DashboardPage::setSession(const SessionData &session)
{
    const QString displayName = session.login.isEmpty()
        ? QString("Пользователь #%1").arg(session.userId)
        : session.login;
    m_welcomeLabel->setText(QString("Добро пожаловать, %1").arg(displayName));
}

void DashboardPage::setCourses(const QVector<CourseData> &courses)
{
    m_coursesValueLabel->setText(QString::number(courses.size()));
    m_recentCoursesList->clear();

    const int maxItems = qMin(5, courses.size());
    for (int i = 0; i < maxItems; ++i) {
        auto *item = new QListWidgetItem(courses[i].title, m_recentCoursesList);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    m_testsValueLabel->setText("—");
}

void DashboardPage::setAttempts(const QVector<AttemptData> &attempts)
{
    if (m_role == "Teacher") {
        m_attemptsValueLabel->setText("—");
        return;
    }

    m_attemptsValueLabel->setText(QString::number(attempts.size()));
}
