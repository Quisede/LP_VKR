#include "dashboardpage.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVariant>
#include <QVBoxLayout>

namespace {
int totalLessons(const QVector<CourseData> &courses)
{
    int sum = 0;
    for (const auto &course : courses) {
        sum += course.lessonsCount;
    }
    return sum;
}

int totalTests(const QVector<CourseData> &courses)
{
    int sum = 0;
    for (const auto &course : courses) {
        sum += course.testsCount;
    }
    return sum;
}

int totalStudents(const QVector<CourseData> &courses)
{
    int sum = 0;
    for (const auto &course : courses) {
        sum += course.studentsCount;
    }
    return sum;
}

int averageProgress(const QVector<CourseData> &courses)
{
    if (courses.isEmpty()) {
        return 0;
    }

    int sum = 0;
    for (const auto &course : courses) {
        sum += course.progressPercent;
    }
    return sum / courses.size();
}

QString teacherCourseStatsLine(const CourseData &course)
{
    return QString("Уроков: %1  •  Тестов: %2  •  Студентов: %3")
        .arg(course.lessonsCount)
        .arg(course.testsCount)
        .arg(course.studentsCount);
}

QString adminCourseStatsLine(const CourseData &course)
{
    return QString("Уроков: %1  •  Тестов: %2  •  Студентов: %3")
        .arg(course.lessonsCount)
        .arg(course.testsCount)
        .arg(course.studentsCount);
}

const AttemptData *latestAttemptPtr(const QVector<AttemptData> &attempts)
{
    if (attempts.isEmpty()) {
        return nullptr;
    }
    return &attempts.last();
}

const CourseData *recommendedStudentCourse(const QVector<CourseData> &courses)
{
    const CourseData *fallback = nullptr;
    for (const CourseData &course : courses) {
        if (fallback == nullptr) {
            fallback = &course;
        }

        if (course.progressPercent > 0 && course.progressPercent < 100) {
            return &course;
        }
    }

    for (const CourseData &course : courses) {
        if (course.progressPercent == 0) {
            return &course;
        }
    }

    return fallback;
}

const CourseData *teacherPriorityCourse(const QVector<CourseData> &courses)
{
    const CourseData *fallback = nullptr;
    for (const CourseData &course : courses) {
        if (fallback == nullptr) {
            fallback = &course;
        }

        if (course.lessonsCount == 0 || course.testsCount == 0) {
            return &course;
        }
    }
    return fallback;
}

void appendInfoCard(QListWidget *list, const QString &title, const QString &subtitle, int courseId = -1)
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 104));
    item->setData(Qt::UserRole, courseId);

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");
    if (courseId >= 0) {
        card->setCursor(Qt::PointingHandCursor);
    }

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

    m_nextActionCard = new QFrame(pageCard);
    m_nextActionCard->setObjectName("moduleCard");
    m_nextActionCard->setMinimumHeight(138);
    m_nextActionCard->setMaximumHeight(168);
    auto *nextActionLayout = new QHBoxLayout(m_nextActionCard);
    nextActionLayout->setContentsMargins(20, 18, 20, 18);
    nextActionLayout->setSpacing(18);

    auto *nextActionTextLayout = new QVBoxLayout();
    nextActionTextLayout->setSpacing(6);
    m_nextActionEyebrowLabel = new QLabel("Продолжить обучение", m_nextActionCard);
    m_nextActionEyebrowLabel->setObjectName("sectionHintLabel");
    m_nextActionTitleLabel = new QLabel("Следующий шаг пока не выбран", m_nextActionCard);
    m_nextActionTitleLabel->setObjectName("courseCardTitleLabel");
    m_nextActionTitleLabel->setWordWrap(true);
    m_nextActionDescriptionLabel = new QLabel("Когда появятся курсы, здесь будет предложено самое полезное действие.", m_nextActionCard);
    m_nextActionDescriptionLabel->setObjectName("courseCardDescriptionLabel");
    m_nextActionDescriptionLabel->setWordWrap(true);
    nextActionTextLayout->addWidget(m_nextActionEyebrowLabel);
    nextActionTextLayout->addWidget(m_nextActionTitleLabel);
    nextActionTextLayout->addWidget(m_nextActionDescriptionLabel);
    nextActionTextLayout->addStretch();

    m_nextActionButton = new QPushButton("Открыть", m_nextActionCard);
    m_nextActionButton->setObjectName("cardAccentButton");
    m_nextActionButton->setMinimumHeight(46);
    m_nextActionButton->setMinimumWidth(180);

    nextActionLayout->addLayout(nextActionTextLayout, 1);
    nextActionLayout->addWidget(m_nextActionButton, 0, Qt::AlignVCenter);

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

    m_actionsTitleLabel = new QLabel("Быстрые действия", focusCard);
    m_actionsTitleLabel->setObjectName("moduleTitleLabel");

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(10);

    auto createActionButton = [focusCard](const QString &text, const QString &objectName) {
        auto *button = new QPushButton(text, focusCard);
        button->setObjectName(objectName);
        button->setMinimumHeight(44);
        return button;
    };

    m_primaryActionButton = createActionButton("Открыть курсы", "cardAccentButton");
    m_secondaryActionButton = createActionButton("Результаты", "cardGhostButton");
    m_tertiaryActionButton = createActionButton("Тесты", "cardGhostButton");
    m_quaternaryActionButton = createActionButton("Профиль", "cardGhostButton");

    focusLayout->addWidget(m_focusTitleLabel);
    focusLayout->addWidget(m_focusList);
    focusLayout->addWidget(m_actionsTitleLabel);
    actionsLayout->addWidget(m_primaryActionButton);
    actionsLayout->addWidget(m_secondaryActionButton);
    actionsLayout->addWidget(m_tertiaryActionButton);
    actionsLayout->addWidget(m_quaternaryActionButton);
    focusLayout->addLayout(actionsLayout);

    contentColumns->addWidget(recentCoursesCard, 3);
    contentColumns->addWidget(focusCard, 2);

    pageLayout->addWidget(m_welcomeLabel);
    pageLayout->addWidget(m_hintLabel);
    pageLayout->addLayout(summaryLayout);
    pageLayout->addWidget(m_nextActionCard);
    pageLayout->addLayout(contentColumns);

    rootLayout->addWidget(pageCard);

    connect(m_primaryActionButton, &QPushButton::clicked, this, [this]() {
        if (m_role == "Teacher") {
            emit openCourseBuilderRequested();
        } else if (m_role == "Admin") {
            emit openUsersRequested();
        } else {
            emit openCoursesRequested();
        }
    });
    connect(m_secondaryActionButton, &QPushButton::clicked, this, [this]() {
        if (m_role == "Teacher") {
            emit openCoursesRequested();
        } else if (m_role == "Admin") {
            emit openAuditRequested();
        } else {
            emit openResultsRequested();
        }
    });
    connect(m_tertiaryActionButton, &QPushButton::clicked, this, [this]() {
        if (m_role == "Teacher") {
            emit openStudentsRequested();
        } else if (m_role == "Admin") {
            emit openAnalyticsRequested();
        } else {
            emit openTestsRequested();
        }
    });
    connect(m_quaternaryActionButton, &QPushButton::clicked, this, [this]() {
        if (m_role == "Teacher") {
            emit openAnalyticsRequested();
        } else if (m_role == "Admin") {
            emit openCoursesRequested();
        } else {
            emit openProfileRequested();
        }
    });
    connect(m_recentCoursesList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        bool ok = false;
        const int courseId = item->data(Qt::UserRole).toInt(&ok);
        if (ok && courseId >= 0) {
            emit openCourseRequested(courseId);
        }
    });
    connect(m_nextActionButton, &QPushButton::clicked, this, [this]() {
        if (m_nextActionCourseId >= 0) {
            emit openCourseRequested(m_nextActionCourseId);
            return;
        }

        if (m_role == "Teacher") {
            emit openCreateCourseRequested();
        } else if (m_role == "Admin") {
            if (m_nextActionButton->text().contains("журнал", Qt::CaseInsensitive)) {
                emit openAuditRequested();
            } else {
                emit openUsersRequested();
            }
        } else {
            emit openCoursesRequested();
        }
    });

    refreshSummary();
    refreshNextAction();
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
        m_testsTitleLabel->setText("Тесты");
        m_attemptsTitleLabel->setText("Студенты");
        m_recentCoursesTitleLabel->setText("Последние курсы преподавателя");
        m_focusTitleLabel->setText("Что сделать дальше");
        m_actionsTitleLabel->setText("Быстрые teacher-действия");
        m_primaryActionButton->setText("Открыть конструктор");
        m_secondaryActionButton->setText("Мои курсы");
        m_tertiaryActionButton->setText("Студенты");
        m_quaternaryActionButton->setText("Аналитика");
    } else if (role == "Admin") {
        m_hintLabel->setText(
            "Это административная панель платформы. Здесь собран срез по пользователям, курсам, контенту и общей структуре платформы.");
        m_coursesTitleLabel->setText("Курсы");
        m_testsTitleLabel->setText("Пользователи");
        m_attemptsTitleLabel->setText("Преподаватели");
        m_recentCoursesTitleLabel->setText("Последние курсы системы");
        m_focusTitleLabel->setText("Системный фокус");
        m_actionsTitleLabel->setText("Быстрые admin-действия");
        m_primaryActionButton->setText("Пользователи");
        m_secondaryActionButton->setText("Журнал");
        m_tertiaryActionButton->setText("Аналитика");
        m_quaternaryActionButton->setText("Курсы");
    } else {
        m_hintLabel->setText(
            "Это стартовый экран платформы. Здесь можно быстро увидеть прогресс, доступные курсы и ближайшие шаги.");
        m_coursesTitleLabel->setText("Курсы");
        m_testsTitleLabel->setText("Средний балл");
        m_attemptsTitleLabel->setText("Попытки");
        m_recentCoursesTitleLabel->setText("Последние доступные курсы");
        m_focusTitleLabel->setText("Что дальше");
        m_actionsTitleLabel->setText("Быстрые действия");
        m_primaryActionButton->setText("Открыть курсы");
        m_secondaryActionButton->setText("Результаты");
        m_tertiaryActionButton->setText("Тесты");
        m_quaternaryActionButton->setText("Профиль");
    }

    refreshSummary();
    refreshNextAction();
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
    refreshNextAction();
    refreshRecentCourses();
    refreshFocus();
}

void DashboardPage::setAttempts(const QVector<AttemptData> &attempts)
{
    m_attempts = attempts;
    refreshSummary();
    refreshNextAction();
    refreshFocus();
}

void DashboardPage::setAdminOverview(const AdminOverviewData &overview)
{
    m_adminOverview = overview;
    if (m_role == "Admin") {
        refreshSummary();
        refreshNextAction();
        refreshFocus();
    }
}

void DashboardPage::refreshSummary()
{
    m_coursesValueLabel->setText(QString::number(m_courses.size()));

    if (m_role == "Teacher") {
        const int lessons = totalLessons(m_courses);
        const int tests = totalTests(m_courses);
        const int students = totalStudents(m_courses);
        m_coursesCaptionLabel->setText(m_courses.isEmpty()
            ? "Пока нет ни одного курса."
            : "Курсы преподавателя.");
        m_testsValueLabel->setText(QString::number(tests));
        m_testsCaptionLabel->setText(m_courses.isEmpty()
            ? "Сначала нужно создать первый курс."
            : QString("Всего уроков в курсах: %1").arg(lessons));
        m_attemptsValueLabel->setText(QString::number(students));
        m_attemptsCaptionLabel->setText(m_courses.isEmpty()
            ? "Пока нет активных направлений."
            : "Студенты, записанные на курсы преподавателя.");
        return;
    } else if (m_role == "Admin") {
        m_coursesValueLabel->setText(QString::number(m_adminOverview.coursesCount));
        m_coursesCaptionLabel->setText(m_adminOverview.coursesCount == 0
            ? "Курсов в системе пока нет."
            : QString("Всего уроков: %1  •  Тестов: %2")
                .arg(m_adminOverview.lessonsCount)
                .arg(m_adminOverview.testsCount));
        m_testsValueLabel->setText(QString::number(m_adminOverview.totalUsers));
        m_testsCaptionLabel->setText(m_adminOverview.totalUsers == 0
            ? "Пользователи появятся после инициализации системы."
            : QString("Студенты: %1  •  Администраторы: %2")
                .arg(m_adminOverview.studentsCount)
                .arg(m_adminOverview.adminsCount));
        m_attemptsValueLabel->setText(QString::number(m_adminOverview.teachersCount));
        m_attemptsCaptionLabel->setText(m_adminOverview.teachersCount == 0
            ? "Пока нет преподавателей в системе."
            : QString("Всего записей на курсы: %1").arg(m_adminOverview.enrollmentsCount));
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
        : QString("Доступно уроков: %1  •  Средний прогресс: %2%")
            .arg(totalLessons(m_courses))
            .arg(averageProgress(m_courses)));
    m_testsValueLabel->setText(QString("%1%").arg(QString::number(average, 'f', 1)));
    m_testsCaptionLabel->setText(m_attempts.isEmpty()
        ? "Средний результат появится после первого теста."
        : "Средний процент по всем попыткам.");
    m_attemptsValueLabel->setText(QString::number(m_attempts.size()));
    m_attemptsCaptionLabel->setText(m_attempts.isEmpty()
        ? "Тесты пока не проходились."
        : QString("Успешно пройдено: %1").arg(passedCount));
}

void DashboardPage::refreshNextAction()
{
    m_nextActionCourseId = -1;

    if (m_role == "Teacher") {
        m_nextActionEyebrowLabel->setText("Рабочий приоритет");
        const CourseData *course = teacherPriorityCourse(m_courses);
        if (course == nullptr) {
            m_nextActionTitleLabel->setText("Создать первый курс");
            m_nextActionDescriptionLabel->setText("После создания курса здесь появится быстрый переход к наполнению уроками, материалами и тестами.");
            m_nextActionButton->setText("Создать курс");
            return;
        }

        m_nextActionCourseId = course->id;
        m_nextActionTitleLabel->setText(course->title);
        if (course->lessonsCount == 0) {
            m_nextActionDescriptionLabel->setText("В курсе пока нет уроков. Лучше начать с базовой структуры и первого учебного материала.");
        } else if (course->testsCount == 0) {
            m_nextActionDescriptionLabel->setText(QString("В курсе уже %1 уроков, но пока нет тестов. Следующий шаг — добавить проверку знаний.")
                .arg(course->lessonsCount));
        } else {
            m_nextActionDescriptionLabel->setText(QString("Курс содержит %1 уроков, %2 тестов и %3 студентов. Можно открыть конструктор для точечной доработки.")
                .arg(course->lessonsCount)
                .arg(course->testsCount)
                .arg(course->studentsCount));
        }
        m_nextActionButton->setText("Открыть конструктор");
        return;
    }

    if (m_role == "Admin") {
        m_nextActionEyebrowLabel->setText("Системный приоритет");
        if (m_adminOverview.totalUsers == 0) {
            m_nextActionTitleLabel->setText("Проверить пользователей");
            m_nextActionDescriptionLabel->setText("Пользователи ещё не загружены в обзор. Раздел пользователей поможет проверить роли и состав системы.");
            m_nextActionButton->setText("Пользователи");
            return;
        }

        m_nextActionTitleLabel->setText("Контроль платформы");
        m_nextActionDescriptionLabel->setText(QString("В системе %1 пользователей, %2 курсов и %3 записей на курсы. Журнал покажет последние административные изменения.")
            .arg(m_adminOverview.totalUsers)
            .arg(m_adminOverview.coursesCount)
            .arg(m_adminOverview.enrollmentsCount));
        m_nextActionButton->setText("Открыть журнал");
        return;
    }

    m_nextActionEyebrowLabel->setText("Продолжить обучение");
    const CourseData *course = recommendedStudentCourse(m_courses);
    if (course == nullptr) {
        m_nextActionTitleLabel->setText("Выбрать первый курс");
        m_nextActionDescriptionLabel->setText("Каталог курсов поможет выбрать направление и перейти к урокам, материалам и тестам.");
        m_nextActionButton->setText("Открыть курсы");
        return;
    }

    m_nextActionCourseId = course->id;
    m_nextActionTitleLabel->setText(course->title);
    if (course->progressPercent >= 100) {
        m_nextActionDescriptionLabel->setText(QString("Курс уже закрыт по тестовому прогрессу. Можно повторить материалы или улучшить результат в тестах."));
    } else if (course->progressPercent == 0) {
        m_nextActionDescriptionLabel->setText(QString("Начните с уроков и материалов. В курсе доступно уроков: %1, тестов: %2.")
            .arg(course->lessonsCount)
            .arg(course->testsCount));
    } else {
        m_nextActionDescriptionLabel->setText(QString("Прогресс курса сейчас %1%. Следующий шаг — открыть курс и продолжить с незавершённых материалов или тестов.")
            .arg(course->progressPercent));
    }
    m_nextActionButton->setText("Продолжить");
}

void DashboardPage::refreshRecentCourses()
{
    m_recentCoursesList->clear();

    if (m_courses.isEmpty()) {
        appendInfoCard(
            m_recentCoursesList,
            m_role == "Teacher" ? "Курсов пока нет" : m_role == "Admin" ? "Системных курсов пока нет" : "Курсы пока не найдены",
            m_role == "Teacher"
                ? "Создание первого курса откроет его в рабочем кабинете."
                : m_role == "Admin"
                    ? "Когда в системе появятся курсы, они сразу станут видны и в административном каталоге."
                : "Когда курсы станут доступны, они появятся здесь.");
        return;
    }

    const int maxItems = qMin(4, m_courses.size());
    for (int i = 0; i < maxItems; ++i) {
        appendInfoCard(
            m_recentCoursesList,
            m_courses[i].title,
            m_role == "Teacher"
                ? QString("%1\n%2")
                    .arg(m_courses[i].description.isEmpty()
                        ? "Курс готов к наполнению и сопровождению."
                        : m_courses[i].description)
                    .arg(teacherCourseStatsLine(m_courses[i]))
                : m_role == "Admin"
                    ? QString("%1\n%2")
                        .arg(m_courses[i].description.isEmpty()
                            ? "Системный обзор структуры курса."
                            : m_courses[i].description)
                        .arg(adminCourseStatsLine(m_courses[i]))
                    : m_courses[i].description.isEmpty()
                        ? QString("В курсе доступны уроки, материалы и тесты.\nУроков: %1  •  Тестов: %2  •  Прогресс: %3%")
                            .arg(m_courses[i].lessonsCount)
                            .arg(m_courses[i].testsCount)
                            .arg(m_courses[i].progressPercent)
                        : QString("%1\nУроков: %2  •  Тестов: %3  •  Прогресс: %4%")
                            .arg(m_courses[i].description)
                            .arg(m_courses[i].lessonsCount)
                            .arg(m_courses[i].testsCount)
                            .arg(m_courses[i].progressPercent),
            m_courses[i].id);
    }
}

void DashboardPage::refreshFocus()
{
    m_focusList->clear();

    if (m_role == "Teacher") {
        if (m_courses.isEmpty()) {
            appendInfoCard(m_focusList, "Создание первого курса", "Начните с раздела \"Создать курс\", чтобы собрать teacher-flow целиком.");
            appendInfoCard(m_focusList, "Подготовка структуры", "После создания курса можно добавить уроки, материалы и тесты.");
            return;
        }

        const int tests = totalTests(m_courses);
        const int lessons = totalLessons(m_courses);
        appendInfoCard(
            m_focusList,
            tests == 0 ? "Первый тест" : "Конструктор курса",
            tests == 0
                ? "В курсах уже " + QString::number(lessons) + " уроков. Следующий сильный шаг — подготовить тесты и проверки знаний."
                : "Структура уже живая: можно уточнить уроки, материалы и сценарии проверки знаний.");
        appendInfoCard(
            m_focusList,
            totalStudents(m_courses) == 0 ? "Первые студенты" : "Студенты и аналитика",
            totalStudents(m_courses) == 0
                ? "Когда появятся записи на курс, здесь начнут отражаться реальные метрики по аудитории."
                : "Разделы студентов и аналитики показывают вовлечённость и результаты по курсам.");
        return;
    } else if (m_role == "Admin") {
        if (m_adminOverview.totalUsers == 0) {
            appendInfoCard(m_focusList, "Первичные пользователи", "Начать лучше с администраторов, преподавателей и студентов, чтобы платформа перестала быть пустой.");
            appendInfoCard(m_focusList, "Первый курс", "После этого можно будет перейти к структуре контента и системной аналитике.");
            return;
        }

        if (m_adminOverview.teachersCount == 0) {
            appendInfoCard(m_focusList, "Преподаватели", "Сейчас в системе нет преподавателей, поэтому курсы некому вести и сопровождать.");
            appendInfoCard(m_focusList, "Роли пользователей", "Раздел пользователей помогает перераспределить роли там, где это уже нужно.");
            return;
        }

        if (m_adminOverview.coursesCount == 0) {
            appendInfoCard(m_focusList, "Запусти первый курс", "Пользователи уже есть, следующий шаг — создать первый курс и подключить teacher-flow.");
            appendInfoCard(m_focusList, "Сверь состав платформы", QString("Сейчас в системе %1 студентов и %2 преподавателей.")
                .arg(m_adminOverview.studentsCount)
                .arg(m_adminOverview.teachersCount));
            return;
        }

        appendInfoCard(
            m_focusList,
            "Контроль структуры",
            QString("В системе уже %1 курсов, %2 уроков и %3 тестов. Раздел курсов покажет, где контент ещё неполный.")
                .arg(m_adminOverview.coursesCount)
                .arg(m_adminOverview.lessonsCount)
                .arg(m_adminOverview.testsCount));
        appendInfoCard(
            m_focusList,
            "Люди и вовлечённость",
            QString("Пользователей: %1. Записей на курсы: %2. Дальше доступны пользователи и аналитика по курсам.")
                .arg(m_adminOverview.totalUsers)
                .arg(m_adminOverview.enrollmentsCount));
        return;
    }

    if (m_courses.isEmpty()) {
        appendInfoCard(m_focusList, "Выбор курса", "Каталог открывает доступные материалы и тесты.");
        return;
    }

    if (m_attempts.isEmpty()) {
        appendInfoCard(m_focusList, "Материалы", "Лучше начать с курса с наибольшим числом уроков и сначала посмотреть основное содержание.");
        appendInfoCard(
            m_focusList,
            "Первый тест",
            QString("Сейчас в доступных курсах %1 тестов. После первой попытки здесь появится реальная учебная динамика.")
                .arg(totalTests(m_courses)));
        return;
    }

    const AttemptData *latestAttempt = latestAttemptPtr(m_attempts);
    appendInfoCard(
        m_focusList,
        latestAttempt && latestAttempt->passed ? "Закрепи результат" : "Вернись к сложному тесту",
        latestAttempt
            ? QString("Последний тест: %1 — %2%. %3")
                .arg(latestAttempt->testTitle.isEmpty() ? "без названия" : latestAttempt->testTitle)
                .arg(QString::number(latestAttempt->percentage, 'f', 1))
                .arg(latestAttempt->passed ? "Можно улучшить результат или перейти к следующему курсу." : "Лучше сначала повторить материалы и пройти тест снова.")
            : "Раздел результатов покажет, какой тест стоит перепройти.");
    appendInfoCard(m_focusList, "Сверяй прогресс", "Раздел результатов теперь показывает средний балл, историю попыток и статус прохождения.");
}
