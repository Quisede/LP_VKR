#include "dashboardpage.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

namespace {
void appendInfoCard(QListWidget *list, const QString &title, const QString &subtitle)
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 84));

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(6);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *subtitleLabel = new QLabel(subtitle, card);
    subtitleLabel->setObjectName("courseCardDescriptionLabel");
    subtitleLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    list->addItem(item);
    list->setItemWidget(item, card);
}

QFrame *createSummaryCard(
    const QString &title,
    QLabel **titleLabelOut,
    QLabel **valueLabel,
    QLabel **captionLabel,
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
    *titleLabelOut = titleLabel;

    *valueLabel = new QLabel("0", card);
    (*valueLabel)->setObjectName("courseDetailTitleLabel");

    *captionLabel = new QLabel(card);
    (*captionLabel)->setObjectName("sectionHintLabel");
    (*captionLabel)->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(*valueLabel);
    layout->addWidget(*captionLabel);
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
    summaryLayout->addWidget(createSummaryCard("Курсы", &m_coursesTitleLabel, &m_coursesValueLabel, &m_coursesCaptionLabel, pageCard));
    summaryLayout->addWidget(createSummaryCard("Тесты", &m_testsTitleLabel, &m_testsValueLabel, &m_testsCaptionLabel, pageCard));
    summaryLayout->addWidget(createSummaryCard("Попытки", &m_attemptsTitleLabel, &m_attemptsValueLabel, &m_attemptsCaptionLabel, pageCard));

    auto *contentColumns = new QHBoxLayout();
    contentColumns->setSpacing(14);

    auto *recentCoursesCard = new QFrame(pageCard);
    recentCoursesCard->setObjectName("moduleCard");
    auto *recentLayout = new QVBoxLayout(recentCoursesCard);
    recentLayout->setContentsMargins(18, 18, 18, 18);
    recentLayout->setSpacing(12);

    m_recentCoursesTitleLabel = new QLabel("Последние доступные курсы", recentCoursesCard);
    m_recentCoursesTitleLabel->setObjectName("moduleTitleLabel");

    m_recentCoursesList = new QListWidget(recentCoursesCard);
    m_recentCoursesList->setSpacing(10);
    m_recentCoursesList->setSelectionMode(QAbstractItemView::NoSelection);
    m_recentCoursesList->setFocusPolicy(Qt::NoFocus);
    recentLayout->addWidget(m_recentCoursesTitleLabel);
    recentLayout->addWidget(m_recentCoursesList);

    auto *focusCard = new QFrame(pageCard);
    focusCard->setObjectName("moduleCard");
    auto *focusLayout = new QVBoxLayout(focusCard);
    focusLayout->setContentsMargins(18, 18, 18, 18);
    focusLayout->setSpacing(12);

    m_focusTitleLabel = new QLabel("Фокус на сегодня", focusCard);
    m_focusTitleLabel->setObjectName("moduleTitleLabel");

    m_focusList = new QListWidget(focusCard);
    m_focusList->setSpacing(10);
    m_focusList->setSelectionMode(QAbstractItemView::NoSelection);
    m_focusList->setFocusPolicy(Qt::NoFocus);

    focusLayout->addWidget(m_focusTitleLabel);
    focusLayout->addWidget(m_focusList);

    contentColumns->addWidget(recentCoursesCard, 3);
    contentColumns->addWidget(focusCard, 2);

    pageLayout->addWidget(m_welcomeLabel);
    pageLayout->addWidget(m_hintLabel);
    pageLayout->addLayout(summaryLayout);
    pageLayout->addLayout(contentColumns);

    rootLayout->addWidget(pageCard);

    refreshSummary();
    refreshRecentCourses();
    refreshFocus();
}

void DashboardPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher") {
        m_hintLabel->setText(
            "Это рабочий кабинет преподавателя. Здесь собрана сводка по курсам, текущему контенту и ближайшим действиям.");
        m_coursesTitleLabel->setText("Мои курсы");
        m_testsTitleLabel->setText("Режим");
        m_attemptsTitleLabel->setText("Фокус");
        m_recentCoursesTitleLabel->setText("Последние курсы преподавателя");
        m_focusTitleLabel->setText("Что сделать дальше");
    } else {
        m_hintLabel->setText(
            "Это стартовый экран платформы. Здесь можно быстро увидеть прогресс, доступные курсы и ближайшие шаги.");
        m_coursesTitleLabel->setText("Курсы");
        m_testsTitleLabel->setText("Средний балл");
        m_attemptsTitleLabel->setText("Попытки");
        m_recentCoursesTitleLabel->setText("Последние доступные курсы");
        m_focusTitleLabel->setText("Что дальше");
    }

    refreshSummary();
    refreshRecentCourses();
    refreshFocus();
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
    m_courses = courses;
    refreshSummary();
    refreshRecentCourses();
    refreshFocus();
}

void DashboardPage::setAttempts(const QVector<AttemptData> &attempts)
{
    m_attempts = attempts;
    refreshSummary();
    refreshFocus();
}

void DashboardPage::refreshSummary()
{
    m_coursesValueLabel->setText(QString::number(m_courses.size()));

    if (m_role == "Teacher") {
        m_coursesCaptionLabel->setText(m_courses.isEmpty()
            ? "Пока нет ни одного курса."
            : "Курсы, где ты преподаватель.");
        m_testsValueLabel->setText(m_courses.isEmpty() ? "Старт" : "Готов");
        m_testsCaptionLabel->setText(m_courses.isEmpty()
            ? "Сначала создай первый курс."
            : "Можно переходить в конструктор.");
        m_attemptsValueLabel->setText(m_courses.isEmpty() ? "0" : QString::number(m_courses.size()));
        m_attemptsCaptionLabel->setText(m_courses.isEmpty()
            ? "Пока нет активных направлений."
            : "Столько курсов можно развивать дальше.");
        return;
    }

    int passedCount = 0;
    double totalPercentage = 0.0;
    for (const AttemptData &attempt : m_attempts) {
        totalPercentage += attempt.percentage;
        if (attempt.passed) {
            ++passedCount;
        }
    }
    const double average = m_attempts.isEmpty() ? 0.0 : totalPercentage / static_cast<double>(m_attempts.size());

    m_coursesCaptionLabel->setText(m_courses.isEmpty()
        ? "Курсы пока недоступны."
        : "Выбирай курс и открывай материалы.");
    m_testsValueLabel->setText(QString("%1%").arg(QString::number(average, 'f', 1)));
    m_testsCaptionLabel->setText(m_attempts.isEmpty()
        ? "Средний результат появится после первого теста."
        : "Средний процент по всем попыткам.");
    m_attemptsValueLabel->setText(QString::number(m_attempts.size()));
    m_attemptsCaptionLabel->setText(m_attempts.isEmpty()
        ? "Ты ещё не проходил тесты."
        : QString("Успешно пройдено: %1").arg(passedCount));
}

void DashboardPage::refreshRecentCourses()
{
    m_recentCoursesList->clear();

    if (m_courses.isEmpty()) {
        appendInfoCard(
            m_recentCoursesList,
            m_role == "Teacher" ? "Курсов пока нет" : "Курсы пока не найдены",
            m_role == "Teacher"
                ? "Создай первый курс, чтобы он появился в рабочем кабинете."
                : "Когда курсы станут доступны, они появятся здесь.");
        return;
    }

    const int maxItems = qMin(4, m_courses.size());
    for (int i = 0; i < maxItems; ++i) {
        appendInfoCard(
            m_recentCoursesList,
            m_courses[i].title,
            m_courses[i].description.isEmpty()
                ? (m_role == "Teacher"
                    ? "Открой курс или перейди в конструктор."
                    : "Открой курс, чтобы посмотреть уроки, материалы и тесты.")
                : m_courses[i].description);
    }
}

void DashboardPage::refreshFocus()
{
    m_focusList->clear();

    if (m_role == "Teacher") {
        if (m_courses.isEmpty()) {
            appendInfoCard(m_focusList, "Создай первый курс", "Начни с карточки \"Создать курс\", чтобы собрать teacher-flow целиком.");
            appendInfoCard(m_focusList, "Подготовь структуру", "После создания курса добавь уроки, материалы и тесты.");
            return;
        }

        appendInfoCard(m_focusList, "Открой конструктор курса", "Уточни структуру уроков, материалов и тестов для выбранного курса.");
        appendInfoCard(m_focusList, "Проверь студентов", "Посмотри, кто уже записан, и открой аналитику по попыткам.");
        return;
    }

    if (m_courses.isEmpty()) {
        appendInfoCard(m_focusList, "Выбери курс", "Перейди в каталог, чтобы открыть доступные материалы и тесты.");
        return;
    }

    if (m_attempts.isEmpty()) {
        appendInfoCard(m_focusList, "Начни с материалов", "Открой любой курс и сначала просмотри уроки и материалы.");
        appendInfoCard(m_focusList, "Пройди первый тест", "После этого в разделе результатов появится история попыток.");
        return;
    }

    appendInfoCard(m_focusList, "Продолжай обучение", "Открой курс с тестами и попробуй улучшить результат прошлых попыток.");
    appendInfoCard(m_focusList, "Сверяй прогресс", "Раздел результатов теперь показывает средний балл и статус прохождения.");
}
