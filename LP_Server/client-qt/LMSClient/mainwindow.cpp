#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "api/apiclient.h"
#include "pages/attemptspage.h"
#include "pages/coursedetailspage.h"
#include "pages/coursespage.h"
#include "pages/dashboardpage.h"
#include "pages/profilepage.h"
#include "pages/teachercoursebuilderpage.h"
#include "pages/teachercreatecoursepage.h"
#include "pages/teacherstudentspage.h"
#include "pages/testrunnerpage.h"

#include <QFrame>
#include <QFormLayout>
#include <QDebug>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <memory>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <utility>
#include <QVBoxLayout>

namespace {
constexpr int kInvalidId = -1;

QPushButton *createSidebarButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setCheckable(true);
    button->setMinimumHeight(46);
    button->setStyleSheet(
        "QPushButton {"
        " border: none;"
        " border-radius: 14px;"
        " padding: 0 16px;"
        " text-align: left;"
        " background-color: rgba(148, 163, 184, 0.14);"
        " color: #e2e8f0;"
        " font-size: 14px;"
        " font-weight: 600;"
        "}"
        "QPushButton:hover {"
        " background-color: rgba(148, 163, 184, 0.24);"
        "}"
        "QPushButton:checked {"
        " background-color: #2563eb;"
        " color: #ffffff;"
        "}");
    return button;
}
}

MainWindow::MainWindow(ApiClient *apiClient, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_apiClient(apiClient)
    , m_dashboardPage(new DashboardPage(this))
    , m_coursesPage(new CoursesPage(this))
    , m_teacherCreateCoursePage(new TeacherCreateCoursePage(this))
    , m_courseDetailsPage(new CourseDetailsPage(this))
    , m_teacherCourseBuilderPage(new TeacherCourseBuilderPage(this))
    , m_testRunnerPage(new TestRunnerPage(this))
    , m_attemptsPage(new AttemptsPage(this))
    , m_teacherStudentsPage(new TeacherStudentsPage(this))
    , m_teacherAnalyticsPage(createTeacherAnalyticsPage())
    , m_profilePage(new ProfilePage(this))
{
    ui->setupUi(this);
    setWindowTitle("LMS Client");

    m_createCourseButton = createSidebarButton("Создать курс", ui->sidebarFrame);
    m_studentsButton = createSidebarButton("Студенты", ui->sidebarFrame);

    m_createCourseButton->setParent(ui->sidebarFrame);
    m_studentsButton->setParent(ui->sidebarFrame);
    ui->sidebarLayout->insertWidget(5, m_createCourseButton);
    ui->sidebarLayout->insertWidget(7, m_studentsButton);

    ui->stackedWidget->addWidget(m_dashboardPage);
    ui->stackedWidget->addWidget(m_coursesPage);
    ui->stackedWidget->addWidget(m_teacherCreateCoursePage);
    ui->stackedWidget->addWidget(m_courseDetailsPage);
    ui->stackedWidget->addWidget(m_teacherCourseBuilderPage);
    ui->stackedWidget->addWidget(m_attemptsPage);
    ui->stackedWidget->addWidget(m_teacherStudentsPage);
    ui->stackedWidget->addWidget(m_teacherAnalyticsPage);
    ui->stackedWidget->addWidget(m_profilePage);

    ui->testCard->hide();
    ui->testPageLayout->addWidget(m_testRunnerPage);

    connect(ui->homeButton, &QPushButton::clicked, this, &MainWindow::showHomePage);
    connect(ui->coursesButton, &QPushButton::clicked, this, &MainWindow::showCoursesPage);
    connect(m_createCourseButton, &QPushButton::clicked, this, &MainWindow::showCreateCoursePage);
    connect(ui->testButton, &QPushButton::clicked, this, &MainWindow::showTestPage);
    connect(ui->resultsButton, &QPushButton::clicked, this, &MainWindow::showResultsPage);
    connect(m_studentsButton, &QPushButton::clicked, this, &MainWindow::showStudentsPage);
    connect(ui->profileButton, &QPushButton::clicked, this, &MainWindow::showProfilePage);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    connect(m_coursesPage, &CoursesPage::courseOpened, this, &MainWindow::onCourseOpened);
    connect(m_coursesPage, &CoursesPage::courseBuilderRequested, this, &MainWindow::onCourseBuilderRequested);
    connect(m_coursesPage, &CoursesPage::enrollRequested, this, &MainWindow::onEnrollRequested);
    connect(m_courseDetailsPage, &CourseDetailsPage::backRequested, this, &MainWindow::onBackToCoursesRequested);
    connect(m_courseDetailsPage, &CourseDetailsPage::enrollRequested, this, [this]() {
        onEnrollRequested(m_selectedCourse.id);
    });
    connect(m_courseDetailsPage, &CourseDetailsPage::testSelected, this, &MainWindow::onTestSelected);
    connect(m_testRunnerPage, &TestRunnerPage::backRequested, this, &MainWindow::onBackFromTestRequested);
    connect(m_testRunnerPage, &TestRunnerPage::submitRequested, this, &MainWindow::onSubmitTestRequested);
    connect(m_teacherCreateCoursePage, &TeacherCreateCoursePage::createCourseRequested, this,
        [this](const QString &title, const QString &description) {
            m_teacherCreateCoursePage->setBusy(true);
            m_teacherCreateCoursePage->showMessage("Создаём курс...", false);
            showStatus("Создаём курс...");

            m_apiClient->createCourse(
                title,
                description,
                this,
                [this](const CourseData &course) {
                    m_teacherCreateCoursePage->setBusy(false);
                    m_teacherCreateCoursePage->clearForm();
                    m_teacherCreateCoursePage->showMessage("Курс создан. Открываю конструктор курса.", false);
                    showStatus(QString("Курс \"%1\" создан").arg(course.title));
                    m_selectedCourse = course;
                    loadCourses();
                    onCourseBuilderRequested(course);
                },
                [this](const QString &error) {
                    m_teacherCreateCoursePage->setBusy(false);
                    m_teacherCreateCoursePage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::createLessonRequested, this,
        [this](int courseId, const QString &title, const QString &content) {
            showStatus("Создаём урок...");
            m_teacherCourseBuilderPage->showMessage("Создаём урок...", false);
            m_apiClient->createLesson(
                courseId,
                title,
                content,
                this,
                [this](const LessonData &) {
                    m_teacherCourseBuilderPage->clearLessonDraft();
                    m_teacherCourseBuilderPage->showMessage("Урок добавлен в курс.", false);
                    showStatus("Урок создан");
                    loadCourseLessonsAndMaterials(m_selectedCourse.id);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::createMaterialRequested, this,
        [this](int lessonId, const QString &title, const QString &type, const QString &content) {
            showStatus("Добавляем материал...");
            m_teacherCourseBuilderPage->showMessage("Добавляем материал...", false);
            m_apiClient->createMaterial(
                lessonId,
                title,
                type,
                content,
                this,
                [this](const MaterialData &) {
                    m_teacherCourseBuilderPage->clearMaterialDraft();
                    m_teacherCourseBuilderPage->showMessage("Материал добавлен.", false);
                    showStatus("Материал создан");
                    loadCourseLessonsAndMaterials(m_selectedCourse.id);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::createTestRequested, this,
        [this](int courseId, const QString &title) {
            showStatus("Создаём тест...");
            m_teacherCourseBuilderPage->showMessage("Создаём тест...", false);
            m_apiClient->createTest(
                courseId,
                title,
                this,
                [this](const TestData &) {
                    m_teacherCourseBuilderPage->clearTestDraft();
                    m_teacherCourseBuilderPage->showMessage("Тест создан. Следующим шагом добавим редактор вопросов.", false);
                    showStatus("Тест создан");
                    loadCourseTests(m_selectedCourse.id);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherStudentsPage, &TeacherStudentsPage::courseSelected, this, [this](int courseId) {
        if (courseId >= 0) {
            loadTeacherCourseStudents(courseId);
        }
    });

    ui->stackedWidget->setCurrentWidget(m_dashboardPage);
    setActiveSection(ui->homeButton);
    m_testRunnerPage->showPlaceholder(
        "Тест не выбран",
        "Открой курс, затем выбери нужный тест кнопкой \"Начать тест\".");
    m_teacherCourseBuilderPage->clearBuilder();
    setHeader(
        "Учебная панель",
        "Здесь можно открыть курсы, зайти внутрь выбранного курса, пройти тесты и посмотреть данные профиля.");
    showStatus("Готово к работе");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setSession(const SessionData &session)
{
    m_session = session;
    m_apiClient->setToken(session.token);

    applyRoleMode();
    m_dashboardPage->setSession(session);
    m_profilePage->setSession(session);

    showStatus("Ты в системе. Подгружаем курсы и историю попыток");
    loadCourses();
    loadAttempts();
    showHomePage();
}

void MainWindow::showHomePage()
{
    ui->stackedWidget->setCurrentWidget(m_dashboardPage);
    setActiveSection(ui->homeButton);
    if (isTeacherMode()) {
        setHeader("Панель преподавателя", "Сводка по курсам преподавателя и базовый рабочий обзор кабинета.");
    } else {
        setHeader("Главная", "Сводка по обучению, доступным курсам и последним данным аккаунта.");
    }
    showStatus("Главная страница открыта");
}

void MainWindow::showCoursesPage()
{
    ui->stackedWidget->setCurrentWidget(m_coursesPage);
    setActiveSection(ui->coursesButton);
    if (isTeacherMode()) {
        setHeader("Мои курсы", "Курсы преподавателя. Открой курс, чтобы перейти к структуре, урокам и тестам.");
    } else {
        setHeader("Курсы", "Каталог курсов. Открой карточку курса, чтобы перейти к урокам, материалам и тестам.");
    }
    showStatus("Раздел курсов открыт");

    if (m_courses.isEmpty() && !m_session.token.isEmpty()) {
        loadCourses();
    }
}

void MainWindow::showCreateCoursePage()
{
    if (!isTeacherMode()) {
        showStatus("Эта страница доступна только преподавателю");
        return;
    }

    ui->stackedWidget->setCurrentWidget(m_teacherCreateCoursePage);
    setActiveSection(m_createCourseButton);
    setHeader("Создать курс", "Оформи новый курс и сразу переходи в конструктор, чтобы наполнить его уроками, материалами и тестами.");
    m_teacherCreateCoursePage->showMessage(
        "Курс после создания сразу появится в разделе \"Мои курсы\".",
        false);
    showStatus("Открыта teacher-страница создания курса");
}

void MainWindow::showTestPage()
{
    if (isTeacherMode()) {
        if (m_selectedCourse.id == kInvalidId) {
            m_teacherCourseBuilderPage->clearBuilder();
            showTeacherCourseBuilderPage();
            showStatus("Сначала выбери курс во вкладке \"Мои курсы\", чтобы открыть конструктор");
        } else {
            showTeacherCourseBuilderPage();
            showStatus("Открыт конструктор выбранного курса");
        }
        return;
    }

    showTestRunnerPage();

    if (m_selectedTest.id == kInvalidId) {
        m_testRunnerPage->showPlaceholder(
            "Тест не выбран",
            "Сначала открой курс и нажми \"Начать тест\" на карточке нужного теста.");
        if (isTeacherMode()) {
            showStatus("В teacher-режиме это пока просмотрщик тестов. Открой курс и выбери тест.");
        } else {
            showStatus("Выбери тест внутри курса, чтобы начать прохождение");
        }
    } else {
        showStatus("Открыт экран прохождения теста");
    }
}

void MainWindow::showResultsPage()
{
    if (isTeacherMode()) {
        ui->stackedWidget->setCurrentWidget(m_teacherAnalyticsPage);
        setActiveSection(ui->resultsButton);
        setHeader("Аналитика", "Здесь позже появятся студенты, результаты тестов и показатели по курсам преподавателя.");
        showStatus("Открыта teacher-страница аналитики");
        return;
    }

    ui->stackedWidget->setCurrentWidget(m_attemptsPage);
    setActiveSection(ui->resultsButton);
    setHeader("Результаты", "История попыток, баллы, проценты и статус прохождения тестов.");
    showStatus("Раздел результатов открыт");
    loadAttempts();
}

void MainWindow::showStudentsPage()
{
    if (!isTeacherMode()) {
        showStatus("Эта страница доступна только преподавателю");
        return;
    }

    ui->stackedWidget->setCurrentWidget(m_teacherStudentsPage);
    setActiveSection(m_studentsButton);
    setHeader("Студенты", "Выбери курс преподавателя и посмотри список записанных студентов и их прогресс.");
    if (m_courses.isEmpty()) {
        m_teacherStudentsPage->showMessage("Сначала нужен хотя бы один курс преподавателя.", false);
    } else if (m_teacherStudentsPage->selectedCourseId() >= 0) {
        loadTeacherCourseStudents(m_teacherStudentsPage->selectedCourseId());
    }
    showStatus("Открыта teacher-страница студентов");
}

void MainWindow::showProfilePage()
{
    ui->stackedWidget->setCurrentWidget(m_profilePage);
    setActiveSection(ui->profileButton);
    setHeader("Профиль", "Основные данные активной сессии пользователя.");
    showStatus("Личный кабинет открыт");
}

void MainWindow::onLogoutClicked()
{
    m_apiClient->setToken(QString());
    m_session = SessionData{};
    m_courses.clear();
    m_attempts.clear();
    m_selectedLessons.clear();
    m_selectedMaterials.clear();
    m_selectedTests.clear();
    m_selectedCourse = CourseData{};
    m_selectedTest = TestData{};
    emit logoutRequested();
}

void MainWindow::onCourseOpened(const CourseData &course)
{
    m_selectedCourse = course;
    m_selectedTest = TestData{};
    m_selectedLessons.clear();
    m_selectedMaterials.clear();
    m_selectedTests.clear();
    refreshSelectedCourseFromCache();
    if (isTeacherMode()) {
        m_courseDetailsPage->setCourse(m_selectedCourse);
        m_courseDetailsPage->showLoadingState();
        showCourseDetailsPage();
    } else {
        m_courseDetailsPage->setCourse(m_selectedCourse);
        m_courseDetailsPage->showLoadingState();
        showCourseDetailsPage();
    }
    showStatus(QString("Открываем курс \"%1\"").arg(m_selectedCourse.title));
    loadCourseContent(m_selectedCourse.id);
}

void MainWindow::onCourseBuilderRequested(const CourseData &course)
{
    if (!isTeacherMode()) {
        onCourseOpened(course);
        return;
    }

    m_selectedCourse = course;
    m_selectedTest = TestData{};
    m_selectedLessons.clear();
    m_selectedMaterials.clear();
    m_selectedTests.clear();
    refreshSelectedCourseFromCache();
    m_teacherCourseBuilderPage->setCourse(m_selectedCourse);
    m_teacherCourseBuilderPage->setLessons({});
    m_teacherCourseBuilderPage->setMaterials({});
    m_teacherCourseBuilderPage->setTests({});
    m_teacherCourseBuilderPage->showMessage("Загружаем структуру курса...", false);
    showTeacherCourseBuilderPage();
    showStatus(QString("Открываем конструктор курса \"%1\"").arg(m_selectedCourse.title));
    loadCourseContent(m_selectedCourse.id);
}

void MainWindow::onEnrollRequested(int courseId)
{
    if (isTeacherMode()) {
        showStatus("В teacher-режиме запись на курс недоступна. Следующим шагом добавим управление курсами.");
        return;
    }

    if (courseId < 0) {
        showStatus("Сначала выбери корректный курс");
        return;
    }

    showStatus("Записываем на курс...");

    m_apiClient->enrollCourse(
        courseId,
        this,
        [this](const QString &message) {
            showStatus(message);
            loadCourses();
        },
        [this](const QString &error) {
            showStatus(error);
            qDebug() << "Enroll error:" << error;
        });
}

void MainWindow::onBackToCoursesRequested()
{
    showCoursesPage();
}

void MainWindow::onTestSelected(int testId, const QString &title)
{
    if (testId < 0) {
        showStatus("Не удалось определить тест для открытия");
        return;
    }

    m_selectedTest = TestData{};
    m_selectedTest.id = testId;
    m_selectedTest.courseId = m_selectedCourse.id;
    m_selectedTest.title = title;

    qDebug() << "Opening test:" << testId << title;
    m_testRunnerPage->setTest(m_selectedTest);
    m_testRunnerPage->showPlaceholder(
        title.isEmpty() ? "Тест" : title,
        "Загружаем вопросы теста...");
    showTestRunnerPage();
    showStatus("Загружаем вопросы теста...");

    m_apiClient->getQuestions(
        testId,
        this,
        [this, testId](const QVector<QuestionData> &questions) {
            if (m_selectedTest.id != testId) {
                return;
            }

            qDebug() << "Questions loaded for test" << testId << ":" << questions.size();
            m_testRunnerPage->setQuestions(questions);

            if (questions.isEmpty()) {
                showStatus("У этого теста пока нет вопросов");
                return;
            }

            showStatus("Тест загружен. Можно отвечать на вопросы");
        },
        [this, testId](const QString &error) {
            if (m_selectedTest.id != testId) {
                return;
            }

            m_testRunnerPage->showPlaceholder(
                m_selectedTest.title.isEmpty() ? "Тест" : m_selectedTest.title,
                error);
            showStatus(error);
            qDebug() << "Load questions error:" << error;
        });
}

void MainWindow::onBackFromTestRequested()
{
    if (m_selectedCourse.id != kInvalidId) {
        showCourseDetailsPage();
        return;
    }

    showCoursesPage();
}

void MainWindow::onSubmitTestRequested(int testId, const QVector<QPair<int, int>> &answers)
{
    if (testId < 0) {
        showStatus("Сначала открой тест");
        return;
    }

    if (answers.isEmpty()) {
        showStatus("Выбери хотя бы один ответ перед отправкой");
        return;
    }

    showStatus("Отправляем ответы...");

    m_apiClient->submitTest(
        testId,
        answers,
        this,
        [this, testId](const AttemptData &result) {
            if (m_selectedTest.id != testId) {
                return;
            }

            AttemptData normalizedResult = result;
            normalizedResult.testId = testId;
            m_testRunnerPage->showResult(normalizedResult);
            showStatus(
                QString("Тест отправлен: %1/%2, %3%")
                    .arg(normalizedResult.score)
                    .arg(normalizedResult.total)
                    .arg(QString::number(normalizedResult.percentage, 'f', 1)));
            loadAttempts();
        },
        [this, testId](const QString &error) {
            if (m_selectedTest.id != testId) {
                return;
            }

            showStatus(error);
            qDebug() << "Submit test error:" << error;
        });
}

void MainWindow::setActiveSection(QPushButton *button)
{
    ui->homeButton->setChecked(button == ui->homeButton);
    ui->coursesButton->setChecked(button == ui->coursesButton);
    m_createCourseButton->setChecked(button == m_createCourseButton);
    ui->testButton->setChecked(button == ui->testButton);
    ui->resultsButton->setChecked(button == ui->resultsButton);
    m_studentsButton->setChecked(button == m_studentsButton);
    ui->profileButton->setChecked(button == ui->profileButton);
}

void MainWindow::setHeader(const QString &title, const QString &subtitle)
{
    ui->pageTitleLabel->setText(title);
    ui->pageSubtitleLabel->setText(subtitle);
}

void MainWindow::showStatus(const QString &status)
{
    ui->statusLabel->setText(status);
}

void MainWindow::showCourseDetailsPage()
{
    if (isTeacherMode()) {
        ui->stackedWidget->setCurrentWidget(m_courseDetailsPage);
        setActiveSection(ui->coursesButton);
        setHeader("Курс", "Обзор выбранного курса преподавателя: уроки, материалы, видео и тесты.");
    } else {
        ui->stackedWidget->setCurrentWidget(m_courseDetailsPage);
        setActiveSection(ui->coursesButton);
        setHeader("Детали курса", "Уроки, материалы, видео и тесты выбранного курса.");
    }
}

void MainWindow::showTeacherCourseBuilderPage()
{
    ui->stackedWidget->setCurrentWidget(m_teacherCourseBuilderPage);
    setActiveSection(ui->testButton);
    setHeader("Конструктор курса", "Собирай курс по шагам: уроки, материалы и тесты внутри teacher-кабинета.");
}

void MainWindow::showTestRunnerPage()
{
    ui->stackedWidget->setCurrentWidget(ui->testPage);
    setActiveSection(ui->testButton);
    if (isTeacherMode()) {
        setHeader("Тесты", "Пока это режим просмотра теста. Позже здесь появится полноценный teacher test editor.");
    } else {
        setHeader("Тесты", "Проходи тест шаг за шагом и сразу отправляй ответы на проверку.");
    }
}

QWidget *MainWindow::createTeacherAnalyticsPage()
{
    auto *page = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(page);
    pageCard->setObjectName("pageCard");

    auto *layout = new QVBoxLayout(pageCard);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto *titleLabel = new QLabel("Аналитика преподавателя", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Здесь позже появится средний результат тестов, количество попыток и общая активность студентов по курсам преподавателя.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);

    for (const QString &title : {"Курсы", "Студенты", "Попытки"}) {
        auto *card = new QFrame(pageCard);
        card->setObjectName("moduleCard");
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 18, 18, 18);
        cardLayout->setSpacing(8);

        auto *cardTitle = new QLabel(title, card);
        cardTitle->setObjectName("moduleTitleLabel");

        auto *valueLabel = new QLabel("—", card);
        valueLabel->setObjectName("courseDetailTitleLabel");

        cardLayout->addWidget(cardTitle);
        cardLayout->addWidget(valueLabel);
        cardLayout->addStretch();
        statsLayout->addWidget(card);
    }

    auto *noteCard = new QFrame(pageCard);
    noteCard->setObjectName("profileInfoCard");
    auto *noteLayout = new QVBoxLayout(noteCard);
    noteLayout->setContentsMargins(18, 18, 18, 18);
    noteLayout->setSpacing(10);

    auto *noteTitle = new QLabel("Следующий backend-шаг", noteCard);
    noteTitle->setObjectName("moduleTitleLabel");

    auto *noteText = new QLabel(
        "Добавим endpoint аналитики курса и затем привяжем сюда реальные данные по студентам, тестам и попыткам.",
        noteCard);
    noteText->setObjectName("sectionHintLabel");
    noteText->setWordWrap(true);

    noteLayout->addWidget(noteTitle);
    noteLayout->addWidget(noteText);

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addLayout(statsLayout);
    layout->addWidget(noteCard);
    rootLayout->addWidget(pageCard);

    return page;
}

void MainWindow::loadCourses()
{
    m_coursesPage->showPlaceholder(
        "Загружаем курсы...",
        "Подтягиваем каталог курсов с сервера.");
    showStatus("Загружаем курсы...");

    m_apiClient->getCourses(
        this,
        [this](const QVector<CourseData> &courses) {
            m_courses = visibleCoursesForCurrentRole(courses);
            m_dashboardPage->setCourses(m_courses);

            if (m_courses.isEmpty()) {
                if (isTeacherMode()) {
                    m_teacherStudentsPage->setCourses({});
                }
                m_coursesPage->showPlaceholder(
                    isTeacherMode() ? "У преподавателя пока нет курсов" : "Курсов пока нет",
                    isTeacherMode()
                        ? "Создай первый курс во вкладке \"Создать курс\", и он сразу появится здесь."
                        : "Сервер пока не вернул ни одного курса. Позже здесь появятся учебные карточки.");
                showStatus("Каталог курсов пока пуст");
                return;
            }

            m_coursesPage->setCourses(m_courses);
            if (isTeacherMode()) {
                m_teacherStudentsPage->setCourses(m_courses);
            }
            refreshSelectedCourseFromCache();

            if (m_selectedCourse.id != kInvalidId) {
                if (isTeacherMode()) {
                    m_teacherCourseBuilderPage->setCourse(m_selectedCourse);
                } else {
                    m_courseDetailsPage->setCourse(m_selectedCourse);
                }
            }

            showStatus("Курсы загружены");
        },
        [this](const QString &error) {
            if (isTeacherMode()) {
                m_teacherStudentsPage->clearStudents();
                m_teacherStudentsPage->showMessage("Не удалось загрузить курсы преподавателя.", true);
            }
            m_coursesPage->showPlaceholder(
                "Не удалось загрузить курсы",
                error);
            showStatus(error);
            qDebug() << "Load courses error:" << error;
        });
}

void MainWindow::loadAttempts()
{
    if (isTeacherMode()) {
        m_attempts.clear();
        m_attemptsPage->showPlaceholder(
            "Teacher analytics будет следующим шагом: здесь появятся студенты, результаты и показатели по курсам.");
        return;
    }

    if (m_session.userId < 0) {
        m_attemptsPage->showError("Не удалось определить пользователя для загрузки истории попыток.");
        return;
    }

    m_apiClient->getAttempts(
        m_session.userId,
        this,
        [this](const QVector<AttemptData> &attempts) {
            m_attempts = attempts;
            m_dashboardPage->setAttempts(attempts);
            m_attemptsPage->setAttempts(attempts);
        },
        [this](const QString &error) {
            m_attemptsPage->showError(error);
            showStatus(error);
            qDebug() << "Load attempts error:" << error;
        });
}

void MainWindow::loadCourseContent(int courseId)
{
    if (courseId < 0) {
        return;
    }

    loadCourseLessonsAndMaterials(courseId);
    loadCourseTests(courseId);
}

void MainWindow::loadCourseLessonsAndMaterials(int courseId)
{
    m_apiClient->getLessons(
        courseId,
        this,
        [this, courseId](const QVector<LessonData> &lessons) {
            if (m_selectedCourse.id != courseId) {
                return;
            }

            m_selectedLessons = lessons;
            if (isTeacherMode()) {
                m_teacherCourseBuilderPage->setLessons(lessons);
            } else {
                m_courseDetailsPage->setLessons(lessons);
            }

            if (lessons.isEmpty()) {
                m_selectedMaterials.clear();
                if (isTeacherMode()) {
                    m_teacherCourseBuilderPage->setMaterials({});
                } else {
                    m_courseDetailsPage->setMaterials({}, {});
                }
                return;
            }

            struct MaterialAccumulator {
                int pending = 0;
                bool hadError = false;
                QVector<MaterialData> materials;
                QVector<MaterialData> videos;
            };

            auto accumulator = std::make_shared<MaterialAccumulator>();
            accumulator->pending = lessons.size();

            for (const LessonData &lesson : lessons) {
                m_apiClient->getMaterials(
                    lesson.id,
                    this,
                    [this, courseId, accumulator](const QVector<MaterialData> &items) {
                        if (m_selectedCourse.id != courseId) {
                            return;
                        }

                        for (const MaterialData &item : items) {
                            if (item.type.compare("video", Qt::CaseInsensitive) == 0) {
                                accumulator->videos.push_back(item);
                            } else {
                                accumulator->materials.push_back(item);
                            }
                        }

                        accumulator->pending -= 1;
                        if (accumulator->pending == 0) {
                            m_selectedMaterials = accumulator->materials;
                            for (const MaterialData &video : std::as_const(accumulator->videos)) {
                                m_selectedMaterials.push_back(video);
                            }
                            if (isTeacherMode()) {
                                m_teacherCourseBuilderPage->setMaterials(m_selectedMaterials);
                            } else {
                                m_courseDetailsPage->setMaterials(
                                    accumulator->materials,
                                    accumulator->videos);
                            }
                            if (!accumulator->hadError) {
                                showStatus("Материалы курса загружены");
                            }
                        }
                    },
                    [this, courseId, accumulator](const QString &error) {
                        if (m_selectedCourse.id != courseId) {
                            return;
                        }

                        accumulator->hadError = true;
                        accumulator->pending -= 1;
                        qDebug() << "Load materials error:" << error;

                        if (accumulator->pending == 0) {
                            m_selectedMaterials = accumulator->materials;
                            for (const MaterialData &video : std::as_const(accumulator->videos)) {
                                m_selectedMaterials.push_back(video);
                            }
                            if (isTeacherMode()) {
                                m_teacherCourseBuilderPage->setMaterials(m_selectedMaterials);
                            } else {
                                m_courseDetailsPage->setMaterials(
                                    accumulator->materials,
                                    accumulator->videos);
                            }
                        }

                        showStatus(error);
                    });
            }
        },
        [this, courseId](const QString &error) {
            if (m_selectedCourse.id != courseId) {
                return;
            }

            m_selectedLessons.clear();
            m_selectedMaterials.clear();
            if (isTeacherMode()) {
                m_teacherCourseBuilderPage->setLessons({});
                m_teacherCourseBuilderPage->setMaterials({});
            } else {
                m_courseDetailsPage->setLessons({});
                m_courseDetailsPage->setMaterials({}, {});
            }
            showStatus(error);
            qDebug() << "Load lessons error:" << error;
        });
}

void MainWindow::loadCourseTests(int courseId)
{
    m_apiClient->getTests(
        courseId,
        this,
        [this, courseId](const QVector<TestData> &tests) {
            if (m_selectedCourse.id != courseId) {
                return;
            }

            m_selectedTests = tests;
            if (isTeacherMode()) {
                m_teacherCourseBuilderPage->setTests(tests);
            } else {
                m_courseDetailsPage->setTests(tests);
            }
            if (tests.isEmpty()) {
                showStatus("Для этого курса тестов пока нет");
                return;
            }

            showStatus("Тесты курса загружены");
        },
        [this, courseId](const QString &error) {
            if (m_selectedCourse.id != courseId) {
                return;
            }

            m_selectedTests.clear();
            if (isTeacherMode()) {
                m_teacherCourseBuilderPage->setTests({});
            } else {
                m_courseDetailsPage->setTests({});
            }
            showStatus(error);
            qDebug() << "Load tests error:" << error;
        });
}

void MainWindow::loadTeacherCourseStudents(int courseId)
{
    if (!isTeacherMode()) {
        return;
    }

    m_teacherStudentsPage->showMessage("Загружаем студентов курса...", false);
    showStatus("Загружаем студентов...");

    m_apiClient->getCourseStudents(
        courseId,
        this,
        [this](const QVector<CourseStudentData> &students) {
            m_teacherStudentsPage->setStudents(students);
            showStatus("Список студентов обновлён");
        },
        [this](const QString &error) {
            m_teacherStudentsPage->clearStudents();
            m_teacherStudentsPage->showMessage(error, true);
            showStatus(error);
            qDebug() << "Load course students error:" << error;
        });
}

void MainWindow::refreshSelectedCourseFromCache()
{
    if (m_selectedCourse.id == kInvalidId) {
        return;
    }

    for (const CourseData &course : std::as_const(m_courses)) {
        if (course.id == m_selectedCourse.id) {
            m_selectedCourse = course;
            return;
        }
    }
}

bool MainWindow::isTeacherMode() const
{
    return m_session.role == "Teacher";
}

void MainWindow::applyRoleMode()
{
    m_dashboardPage->setRoleMode(m_session.role);
    m_coursesPage->setRoleMode(m_session.role);
    m_courseDetailsPage->setRoleMode(m_session.role);
    m_attemptsPage->setRoleMode(m_session.role);

    if (isTeacherMode()) {
        ui->brandLabel->setText("LMS Teacher");
        ui->brandCaptionLabel->setText("Рабочее пространство преподавателя");
        ui->homeButton->setText("Панель");
        ui->coursesButton->setText("Мои курсы");
        m_createCourseButton->show();
        ui->testButton->setText("Конструктор курса");
        ui->resultsButton->setText("Аналитика");
        m_studentsButton->show();
        ui->profileButton->setText("Профиль");
    } else {
        ui->brandLabel->setText("LMS Client");
        ui->brandCaptionLabel->setText("Рабочее пространство студента");
        ui->homeButton->setText("Главная");
        ui->coursesButton->setText("Курсы");
        m_createCourseButton->hide();
        ui->testButton->setText("Тесты");
        ui->resultsButton->setText("Результаты");
        m_studentsButton->hide();
        ui->profileButton->setText("Личный кабинет");
    }
}

QVector<CourseData> MainWindow::visibleCoursesForCurrentRole(const QVector<CourseData> &courses) const
{
    if (!isTeacherMode()) {
        return courses;
    }

    QVector<CourseData> filtered;
    for (const CourseData &course : courses) {
        if (course.teacherId == m_session.userId) {
            filtered.push_back(course);
        }
    }
    return filtered;
}
