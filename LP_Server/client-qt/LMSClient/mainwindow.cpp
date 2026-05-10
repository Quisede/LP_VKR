#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "api/apiclient.h"
#include "pages/adminuserspage.h"
#include "pages/attemptspage.h"
#include "pages/coursedetailspage.h"
#include "pages/coursespage.h"
#include "pages/dashboardpage.h"
#include "pages/profilepage.h"
#include "pages/teacheranalyticspage.h"
#include "pages/teachercoursebuilderpage.h"
#include "pages/teachercreatecoursepage.h"
#include "pages/teacherstudentspage.h"
#include "pages/teachertesteditorpage.h"
#include "pages/testrunnerpage.h"

#include <algorithm>
#include <QFrame>
#include <QFormLayout>
#include <QDebug>
#include <QDateTime>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <memory>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QTabWidget>
#include <QTimer>
#include <utility>
#include <QVBoxLayout>

namespace {
constexpr int kInvalidId = -1;

bool confirmDangerAction(
    QWidget *parent,
    const QString &title,
    const QString &text)
{
    return QMessageBox::question(
               parent,
               title,
               text,
               QMessageBox::Yes | QMessageBox::No,
               QMessageBox::No) == QMessageBox::Yes;
}

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
    , m_adminUsersPage(new AdminUsersPage(this))
    , m_dashboardPage(new DashboardPage(this))
    , m_coursesPage(new CoursesPage(this))
    , m_teacherCreateCoursePage(new TeacherCreateCoursePage(this))
    , m_courseDetailsPage(new CourseDetailsPage(this))
    , m_teacherCourseBuilderPage(new TeacherCourseBuilderPage(this))
    , m_teacherTestEditorPage(new TeacherTestEditorPage(this))
    , m_testRunnerPage(new TestRunnerPage(this))
    , m_attemptsPage(new AttemptsPage(this))
    , m_teacherStudentsPage(new TeacherStudentsPage(this))
    , m_teacherAnalyticsPage(new TeacherAnalyticsPage(this))
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
    ui->stackedWidget->addWidget(m_adminUsersPage);
    ui->stackedWidget->addWidget(m_coursesPage);
    ui->stackedWidget->addWidget(m_teacherCreateCoursePage);
    ui->stackedWidget->addWidget(m_courseDetailsPage);
    ui->stackedWidget->addWidget(m_teacherCourseBuilderPage);
    ui->stackedWidget->addWidget(m_teacherTestEditorPage);
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
    connect(m_dashboardPage, &DashboardPage::openCoursesRequested, this, &MainWindow::showCoursesPage);
    connect(m_dashboardPage, &DashboardPage::openCreateCourseRequested, this, &MainWindow::showCreateCoursePage);
    connect(m_dashboardPage, &DashboardPage::openStudentsRequested, this, &MainWindow::showStudentsPage);
    connect(m_dashboardPage, &DashboardPage::openAnalyticsRequested, this, &MainWindow::showResultsPage);
    connect(m_dashboardPage, &DashboardPage::openResultsRequested, this, &MainWindow::showResultsPage);
    connect(m_dashboardPage, &DashboardPage::openTestsRequested, this, &MainWindow::showTestPage);
    connect(m_dashboardPage, &DashboardPage::openUsersRequested, this, [this]() {
        if (!isAdminMode()) {
            return;
        }

        ui->stackedWidget->setCurrentWidget(m_adminUsersPage);
        setActiveSection(ui->homeButton);
        setHeader("Пользователи", "Управление ролями, поиском и жизненным циклом пользователей платформы.");
        showStatus("Раздел пользователей открыт");
        loadAdminUsers();
    });

    connect(m_coursesPage, &CoursesPage::courseOpened, this, &MainWindow::onCourseOpened);
    connect(m_coursesPage, &CoursesPage::courseBuilderRequested, this, &MainWindow::onCourseBuilderRequested);
    connect(m_coursesPage, &CoursesPage::courseEditRequested, this, [this](const CourseData &course) {
        if (!isTeacherMode() && !isAdminMode()) {
            return;
        }

        m_selectedCourse = course;
        m_teacherCreateCoursePage->setEditMode(course);
        ui->stackedWidget->setCurrentWidget(m_teacherCreateCoursePage);
        setActiveSection(isAdminMode() ? ui->coursesButton : m_createCourseButton);
        setHeader("Редактировать курс", "Обновляй описание курса или удаляй его, если он больше не нужен.");
        showStatus(QString("Редактируем курс \"%1\"").arg(course.title));
    });
    connect(m_coursesPage, &CoursesPage::courseDeleteRequested, this, [this](const CourseData &course) {
        if (!isTeacherMode() && !isAdminMode()) {
            return;
        }

        if (!confirmDangerAction(
                this,
                "Удаление курса",
                QString("Удалить курс \"%1\" вместе с уроками, материалами и тестами?")
                    .arg(course.title))) {
            return;
        }

        m_teacherCreateCoursePage->setBusy(true);
        m_teacherCreateCoursePage->showMessage("Удаляем курс и связанную структуру...", false);
        showStatus("Удаляем курс...");

        m_apiClient->deleteCourse(
            course.id,
            this,
            [this, course]() {
                m_teacherCreateCoursePage->setBusy(false);
                m_teacherCreateCoursePage->setCreateMode();
                if (m_selectedCourse.id == course.id) {
                    m_selectedCourse = CourseData{};
                    m_selectedLessons.clear();
                    m_selectedMaterials.clear();
                    m_selectedTests.clear();
                    m_selectedQuestions.clear();
                    m_teacherCourseBuilderPage->clearBuilder();
                    m_teacherTestEditorPage->clearEditor();
                }
                showStatus(QString("Курс \"%1\" удалён").arg(course.title));
                loadCourses();
                showCoursesPage();
            },
            [this](const QString &error) {
                m_teacherCreateCoursePage->setBusy(false);
                m_teacherCreateCoursePage->showMessage(error, true);
                showStatus(error);
            });
    });
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
    connect(m_teacherCreateCoursePage, &TeacherCreateCoursePage::updateCourseRequested, this,
        [this](int courseId, const QString &title, const QString &description) {
            m_teacherCreateCoursePage->setBusy(true);
            m_teacherCreateCoursePage->showMessage("Сохраняем изменения курса...", false);
            showStatus("Обновляем курс...");

            m_apiClient->updateCourse(
                courseId,
                title,
                description,
                this,
                [this](const CourseData &course) {
                    m_teacherCreateCoursePage->setBusy(false);
                    m_teacherCreateCoursePage->setEditMode(course);
                    m_teacherCreateCoursePage->showMessage("Курс обновлён. Изменения сохранены.", false);
                    showStatus(QString("Курс \"%1\" обновлён").arg(course.title));
                    m_selectedCourse = course;
                    m_teacherCourseBuilderPage->setCourse(course);
                    m_teacherTestEditorPage->setCourse(course);
                    m_courseDetailsPage->setCourse(course);
                    loadCourses();
                },
                [this](const QString &error) {
                    m_teacherCreateCoursePage->setBusy(false);
                    m_teacherCreateCoursePage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCreateCoursePage, &TeacherCreateCoursePage::deleteCourseRequested, this,
        [this](int courseId) {
            if (!confirmDangerAction(
                    this,
                    "Удаление курса",
                    "Удалить курс вместе с уроками, материалами и тестами?")) {
                return;
            }

            m_teacherCreateCoursePage->setBusy(true);
            m_teacherCreateCoursePage->showMessage("Удаляем курс и связанную структуру...", false);
            showStatus("Удаляем курс...");

            m_apiClient->deleteCourse(
                courseId,
                this,
                [this]() {
                    m_teacherCreateCoursePage->setBusy(false);
                    m_teacherCreateCoursePage->setCreateMode();
                    m_selectedCourse = CourseData{};
                    m_selectedLessons.clear();
                    m_selectedMaterials.clear();
                    m_selectedTests.clear();
                    m_selectedQuestions.clear();
                    m_teacherCourseBuilderPage->clearBuilder();
                    m_teacherTestEditorPage->clearEditor();
                    showStatus("Курс удалён");
                    loadCourses();
                    showCoursesPage();
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
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::updateLessonRequested, this,
        [this](int lessonId, const QString &title, const QString &content) {
            showStatus("Обновляем урок...");
            m_teacherCourseBuilderPage->showMessage("Сохраняем изменения урока...", false);
            m_apiClient->updateLesson(
                lessonId,
                title,
                content,
                this,
                [this](const LessonData &) {
                    m_teacherCourseBuilderPage->clearLessonDraft();
                    m_teacherCourseBuilderPage->showMessage("Урок обновлён.", false);
                    showStatus("Урок обновлён");
                    loadCourseLessonsAndMaterials(m_selectedCourse.id);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::deleteLessonRequested, this,
        [this](int lessonId) {
            if (!confirmDangerAction(
                    this,
                    "Удаление урока",
                    "Удалить урок и все материалы, которые к нему привязаны?")) {
                return;
            }

            showStatus("Удаляем урок...");
            m_teacherCourseBuilderPage->showMessage("Удаляем урок и связанные материалы...", false);
            m_apiClient->deleteLesson(
                lessonId,
                this,
                [this]() {
                    m_teacherCourseBuilderPage->clearLessonDraft();
                    m_teacherCourseBuilderPage->clearMaterialDraft();
                    m_teacherCourseBuilderPage->showMessage("Урок удалён.", false);
                    showStatus("Урок удалён");
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
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::updateMaterialRequested, this,
        [this](int materialId, const QString &title, const QString &type, const QString &content) {
            showStatus("Обновляем материал...");
            m_teacherCourseBuilderPage->showMessage("Сохраняем изменения материала...", false);
            m_apiClient->updateMaterial(
                materialId,
                title,
                type,
                content,
                this,
                [this](const MaterialData &) {
                    m_teacherCourseBuilderPage->clearMaterialDraft();
                    m_teacherCourseBuilderPage->showMessage("Материал обновлён.", false);
                    showStatus("Материал обновлён");
                    loadCourseLessonsAndMaterials(m_selectedCourse.id);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::deleteMaterialRequested, this,
        [this](int materialId) {
            if (!confirmDangerAction(
                    this,
                    "Удаление материала",
                    "Удалить выбранный материал?")) {
                return;
            }

            showStatus("Удаляем материал...");
            m_teacherCourseBuilderPage->showMessage("Удаляем материал...", false);
            m_apiClient->deleteMaterial(
                materialId,
                this,
                [this]() {
                    m_teacherCourseBuilderPage->clearMaterialDraft();
                    m_teacherCourseBuilderPage->showMessage("Материал удалён.", false);
                    showStatus("Материал удалён");
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
                [this](const TestData &test) {
                    m_teacherCourseBuilderPage->clearTestDraft();
                    m_teacherCourseBuilderPage->showMessage("Тест создан. Теперь можно добавлять вопросы и варианты ответов.", false);
                    showStatus("Тест создан");
                    loadCourseTests(m_selectedCourse.id);
                    loadManagedQuestions(test.id);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::updateTestRequested, this,
        [this](int testId, const QString &title) {
            showStatus("Обновляем тест...");
            m_teacherCourseBuilderPage->showMessage("Сохраняем изменения теста...", false);
            m_apiClient->updateTest(
                testId,
                title,
                this,
                [this, testId](const TestData &) {
                    m_teacherCourseBuilderPage->clearTestDraft();
                    m_teacherCourseBuilderPage->showMessage("Тест обновлён.", false);
                    showStatus("Тест обновлён");
                    loadCourseTests(m_selectedCourse.id);
                    loadManagedQuestions(testId);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::deleteTestRequested, this,
        [this](int testId) {
            if (!confirmDangerAction(
                    this,
                    "Удаление теста",
                    "Удалить тест и все вопросы, которые в него входят?")) {
                return;
            }

            showStatus("Удаляем тест...");
            m_teacherCourseBuilderPage->showMessage("Удаляем тест и связанные вопросы...", false);
            m_apiClient->deleteTest(
                testId,
                this,
                [this]() {
                    m_teacherCourseBuilderPage->clearTestDraft();
                    m_teacherCourseBuilderPage->clearQuestionDraft();
                    m_teacherCourseBuilderPage->setQuestions({});
                    m_teacherCourseBuilderPage->showMessage("Тест удалён.", false);
                    showStatus("Тест удалён");
                    loadCourseTests(m_selectedCourse.id);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::testSelectedForQuestions, this,
        [this](int testId) {
            if (testId >= 0) {
                loadManagedQuestions(testId);
            }
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::createQuestionRequested, this,
        [this](int testId, const QString &text, const QStringList &options, int correctOptionIndex) {
            showStatus("Сохраняем вопрос...");
            m_teacherCourseBuilderPage->showMessage("Сохраняем вопрос и варианты ответов...", false);
            m_apiClient->createQuestion(
                testId,
                text,
                options,
                correctOptionIndex,
                this,
                [this, testId](const QuestionData &) {
                    m_teacherCourseBuilderPage->clearQuestionDraft();
                    m_teacherCourseBuilderPage->showMessage("Вопрос добавлен в тест.", false);
                    showStatus("Вопрос сохранён");
                    loadManagedQuestions(testId);
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::updateQuestionRequested, this,
        [this](int questionId, const QString &text, const QStringList &options, int correctOptionIndex) {
            showStatus("Обновляем вопрос...");
            m_teacherCourseBuilderPage->showMessage("Сохраняем изменения вопроса...", false);
            m_apiClient->updateQuestion(
                questionId,
                text,
                options,
                correctOptionIndex,
                this,
                [this](const QuestionData &) {
                    m_teacherCourseBuilderPage->clearQuestionDraft();
                    m_teacherCourseBuilderPage->showMessage("Вопрос обновлён.", false);
                    showStatus("Вопрос обновлён");
                    loadManagedQuestions(m_teacherCourseBuilderPage->currentManagedTestId());
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::deleteQuestionRequested, this,
        [this](int questionId) {
            if (!confirmDangerAction(
                    this,
                    "Удаление вопроса",
                    "Удалить выбранный вопрос из теста?")) {
                return;
            }

            showStatus("Удаляем вопрос...");
            m_teacherCourseBuilderPage->showMessage("Удаляем вопрос...", false);
            m_apiClient->deleteQuestion(
                questionId,
                this,
                [this]() {
                    m_teacherCourseBuilderPage->clearQuestionDraft();
                    m_teacherCourseBuilderPage->showMessage("Вопрос удалён.", false);
                    showStatus("Вопрос удалён");
                    loadManagedQuestions(m_teacherCourseBuilderPage->currentManagedTestId());
                },
                [this](const QString &error) {
                    m_teacherCourseBuilderPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherCourseBuilderPage, &TeacherCourseBuilderPage::openDedicatedTestEditorRequested, this, [this]() {
        showTeacherTestEditorPage();
    });
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::backRequested, this, &MainWindow::showTeacherCourseBuilderPage);
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::testSelectedForQuestions, this, [this](int testId) {
        if (testId >= 0) {
            loadManagedQuestions(testId);
        }
    });
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::createTestRequested, this,
        [this](int courseId, const QString &title) {
            showStatus("Создаём тест...");
            m_teacherTestEditorPage->showMessage("Создаём тест...", false);
            m_apiClient->createTest(
                courseId,
                title,
                this,
                [this](const TestData &test) {
                    m_teacherTestEditorPage->clearTestDraft();
                    m_teacherTestEditorPage->showMessage("Тест создан. Теперь можно добавлять вопросы.", false);
                    showStatus("Тест создан");
                    loadCourseTests(m_selectedCourse.id);
                    loadManagedQuestions(test.id);
                },
                [this](const QString &error) {
                    m_teacherTestEditorPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::updateTestRequested, this,
        [this](int testId, const QString &title) {
            showStatus("Обновляем тест...");
            m_teacherTestEditorPage->showMessage("Сохраняем изменения теста...", false);
            m_apiClient->updateTest(
                testId,
                title,
                this,
                [this, testId](const TestData &) {
                    m_teacherTestEditorPage->clearTestDraft();
                    m_teacherTestEditorPage->showMessage("Тест обновлён.", false);
                    showStatus("Тест обновлён");
                    loadCourseTests(m_selectedCourse.id);
                    loadManagedQuestions(testId);
                },
                [this](const QString &error) {
                    m_teacherTestEditorPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::deleteTestRequested, this,
        [this](int testId) {
            if (!confirmDangerAction(
                    this,
                    "Удаление теста",
                    "Удалить тест и все вопросы, которые в него входят?")) {
                return;
            }

            showStatus("Удаляем тест...");
            m_teacherTestEditorPage->showMessage("Удаляем тест и связанные вопросы...", false);
            m_apiClient->deleteTest(
                testId,
                this,
                [this]() {
                    m_teacherTestEditorPage->clearTestDraft();
                    m_teacherTestEditorPage->clearQuestionDraft();
                    m_teacherTestEditorPage->setQuestions({});
                    m_teacherTestEditorPage->showMessage("Тест удалён.", false);
                    showStatus("Тест удалён");
                    loadCourseTests(m_selectedCourse.id);
                },
                [this](const QString &error) {
                    m_teacherTestEditorPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::createQuestionRequested, this,
        [this](int testId, const QString &text, const QStringList &options, int correctOptionIndex) {
            showStatus("Сохраняем вопрос...");
            m_teacherTestEditorPage->showMessage("Сохраняем вопрос и варианты ответов...", false);
            m_apiClient->createQuestion(
                testId,
                text,
                options,
                correctOptionIndex,
                this,
                [this, testId](const QuestionData &) {
                    m_teacherTestEditorPage->clearQuestionDraft();
                    m_teacherTestEditorPage->showMessage("Вопрос добавлен в тест.", false);
                    showStatus("Вопрос сохранён");
                    loadManagedQuestions(testId);
                },
                [this](const QString &error) {
                    m_teacherTestEditorPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::updateQuestionRequested, this,
        [this](int questionId, const QString &text, const QStringList &options, int correctOptionIndex) {
            showStatus("Обновляем вопрос...");
            m_teacherTestEditorPage->showMessage("Сохраняем изменения вопроса...", false);
            m_apiClient->updateQuestion(
                questionId,
                text,
                options,
                correctOptionIndex,
                this,
                [this](const QuestionData &) {
                    m_teacherTestEditorPage->clearQuestionDraft();
                    m_teacherTestEditorPage->showMessage("Вопрос обновлён.", false);
                    showStatus("Вопрос обновлён");
                    loadManagedQuestions(m_teacherTestEditorPage->currentManagedTestId());
                },
                [this](const QString &error) {
                    m_teacherTestEditorPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherTestEditorPage, &TeacherTestEditorPage::deleteQuestionRequested, this,
        [this](int questionId) {
            if (!confirmDangerAction(
                    this,
                    "Удаление вопроса",
                    "Удалить выбранный вопрос из теста?")) {
                return;
            }

            showStatus("Удаляем вопрос...");
            m_teacherTestEditorPage->showMessage("Удаляем вопрос...", false);
            m_apiClient->deleteQuestion(
                questionId,
                this,
                [this]() {
                    m_teacherTestEditorPage->clearQuestionDraft();
                    m_teacherTestEditorPage->showMessage("Вопрос удалён.", false);
                    showStatus("Вопрос удалён");
                    loadManagedQuestions(m_teacherTestEditorPage->currentManagedTestId());
                },
                [this](const QString &error) {
                    m_teacherTestEditorPage->showMessage(error, true);
                    showStatus(error);
                });
        });
    connect(m_teacherStudentsPage, &TeacherStudentsPage::courseSelected, this, [this](int courseId) {
        if (courseId >= 0) {
            loadTeacherCourseStudents(courseId);
        }
    });
    connect(m_teacherStudentsPage, &TeacherStudentsPage::studentSelected, this, [this](int studentId, const QString &studentLogin) {
        const int courseId = m_teacherStudentsPage->selectedCourseId();
        if (courseId >= 0 && studentId >= 0) {
            loadTeacherStudentAttempts(courseId, studentId, studentLogin);
        }
    });
    connect(m_teacherAnalyticsPage, &TeacherAnalyticsPage::courseSelected, this, [this](int courseId) {
        if (courseId >= 0) {
            loadTeacherAnalytics(courseId);
        }
    });
    connect(m_adminUsersPage, &AdminUsersPage::createUserRequested, this, [this](const QString &login, const QString &password, const QString &role) {
        if (!isAdminMode()) {
            return;
        }

        if (login.isEmpty() || password.isEmpty()) {
            m_adminUsersPage->showMessage("Заполни логин и пароль для нового пользователя.", true);
            showStatus("Нужно заполнить логин и пароль");
            return;
        }

        m_adminUsersPage->setBusy(true);
        m_adminUsersPage->showMessage("Создаём пользователя...", false);
        showStatus("Создаём пользователя...");

        m_apiClient->createAdminUser(
            login,
            password,
            role,
            this,
            [this](const AdminUserData &user) {
                m_adminUsersPage->setBusy(false);
                m_adminUsersPage->clearDraft();
                m_adminUsersPage->showMessage(
                    QString("Пользователь %1 с ролью %2 создан.").arg(user.login, user.role),
                    false);
                showStatus(QString("Создан пользователь %1").arg(user.login));
                loadAdminUsers();
                loadCourses();
            },
            [this](const QString &error) {
                m_adminUsersPage->setBusy(false);
                m_adminUsersPage->showMessage(error, true);
                showStatus(error);
            });
    });
    connect(m_adminUsersPage, &AdminUsersPage::updateUserRoleRequested, this, [this](int userId, const QString &role) {
        if (!isAdminMode()) {
            return;
        }

        m_adminUsersPage->setBusy(true);
        m_adminUsersPage->showMessage("Обновляем роль пользователя...", false);
        showStatus("Обновляем роль пользователя...");

        m_apiClient->updateAdminUserRole(
            userId,
            role,
            this,
            [this](const AdminUserData &user) {
                m_adminUsersPage->setBusy(false);
                m_adminUsersPage->showMessage(
                    QString("Роль пользователя %1 обновлена на %2.").arg(user.login, user.role),
                    false);
                showStatus(QString("Роль обновлена для %1").arg(user.login));
                loadAdminUsers();
            },
            [this](const QString &error) {
                m_adminUsersPage->setBusy(false);
                m_adminUsersPage->showMessage(error, true);
                showStatus(error);
            });
    });
    connect(m_adminUsersPage, &AdminUsersPage::deleteUserRequested, this, [this](int userId) {
        if (!isAdminMode()) {
            return;
        }

        m_adminUsersPage->setBusy(true);
        m_adminUsersPage->showMessage("Удаляем пользователя...", false);
        showStatus("Удаляем пользователя...");

        m_apiClient->deleteAdminUser(
            userId,
            this,
            [this]() {
                m_adminUsersPage->setBusy(false);
                m_adminUsersPage->showMessage("Пользователь удалён.", false);
                showStatus("Пользователь удалён");
                loadAdminUsers();
            },
            [this](const QString &error) {
                m_adminUsersPage->setBusy(false);
                m_adminUsersPage->showMessage(error, true);
                showStatus(error);
            });
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
    m_adminOverview = AdminOverviewData{};

    applyRoleMode();
    m_dashboardPage->setSession(session);
    m_dashboardPage->setAdminOverview(m_adminOverview);
    m_profilePage->setSession(session);

    showStatus("Ты в системе. Подгружаем курсы и историю попыток");
    loadCourses();
    if (isAdminMode()) {
        loadAdminOverview();
    }
    loadAttempts();
    showHomePage();
}

CourseData MainWindow::automationCourseCandidate(const QString &courseTitle) const
{
    if (!courseTitle.trimmed().isEmpty()) {
        for (const CourseData &course : m_courses) {
            if (course.title.compare(courseTitle.trimmed(), Qt::CaseInsensitive) == 0) {
                return course;
            }
        }
    }

    if (!m_courses.isEmpty()) {
        return m_courses.first();
    }

    return CourseData{};
}

void MainWindow::automationOpenPage(const QString &pageKey, const QString &courseTitle)
{
    const QString key = pageKey.trimmed().toLower();

    if (key == "home" || key == "dashboard") {
        showHomePage();
        return;
    }

    if (key == "courses") {
        showCoursesPage();
        return;
    }

    if (key == "results" || key == "attempts") {
        showResultsPage();
        return;
    }

    if (key == "profile") {
        showProfilePage();
        return;
    }

    if (key == "students") {
        showStudentsPage();
        return;
    }

    if (key == "analytics") {
        showResultsPage();
        return;
    }

    if (key == "users" || key == "admin-users") {
        if (isAdminMode()) {
            ui->stackedWidget->setCurrentWidget(m_adminUsersPage);
            setActiveSection(ui->testButton);
            setHeader("Пользователи", "Автоматический просмотр реестра пользователей платформы.");
            loadAdminUsers();
        }
        return;
    }

    if (key == "create-course") {
        showCreateCoursePage();
        return;
    }

    const CourseData course = automationCourseCandidate(courseTitle);
    if (course.id == kInvalidId) {
        showHomePage();
        showStatus("Automation: курс не найден, остаёмся на главной");
        return;
    }

    if (key == "course-details" || key == "details") {
        onCourseOpened(course);
        return;
    }

    if (key == "builder" || key == "course-builder") {
        onCourseBuilderRequested(course);
        return;
    }

    if (key == "test-editor" || key == "tests-editor") {
        onCourseBuilderRequested(course);
        QTimer::singleShot(900, this, [this]() {
            showTeacherTestEditorPage();
        });
        return;
    }

    showHomePage();
}

void MainWindow::showHomePage()
{
    ui->stackedWidget->setCurrentWidget(m_dashboardPage);
    setActiveSection(ui->homeButton);
    if (isTeacherMode()) {
        setHeader("Панель преподавателя", "Сводка по курсам преподавателя и базовый рабочий обзор кабинета.");
    } else if (isAdminMode()) {
        setHeader("Панель администратора", "Общий обзор системы: пользователи, курсы и административные рабочие точки.");
    } else {
        setHeader("Главная", "Сводка по обучению, доступным курсам и последним данным аккаунта.");
    }
    if (isAdminMode()) {
        loadAdminOverview();
    }
    showStatus("Главная страница открыта");
}

void MainWindow::showCoursesPage()
{
    ui->stackedWidget->setCurrentWidget(m_coursesPage);
    setActiveSection(ui->coursesButton);
    if (isTeacherMode()) {
        setHeader("Мои курсы", "Курсы преподавателя. Открой курс, чтобы перейти к структуре, урокам и тестам.");
    } else if (isAdminMode()) {
        setHeader("Курсы системы", "Полный список курсов платформы. Здесь администратор видит общую структуру без student-flow.");
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
    if (!isTeacherMode() && !isAdminMode()) {
        showStatus("Эта страница доступна только преподавателю и администратору");
        return;
    }

    m_teacherCreateCoursePage->setCreateMode();
    ui->stackedWidget->setCurrentWidget(m_teacherCreateCoursePage);
    setActiveSection(isAdminMode() ? ui->coursesButton : m_createCourseButton);
    setHeader("Создать курс", "Оформи новый курс и сразу переходи в конструктор, чтобы наполнить его уроками, материалами и тестами.");
    m_teacherCreateCoursePage->showMessage(
        "Курс после создания сразу появится в разделе \"Мои курсы\".",
        false);
    showStatus(isAdminMode() ? "Открыта admin-страница создания курса" : "Открыта teacher-страница создания курса");
}

void MainWindow::showTestPage()
{
    if (isAdminMode()) {
        ui->stackedWidget->setCurrentWidget(m_adminUsersPage);
        setActiveSection(ui->testButton);
        setHeader("Пользователи", "Реестр пользователей системы: роли, состав платформы и база для дальнейших admin-инструментов.");
        loadAdminUsers();
        showStatus("Открыт admin-раздел пользователей");
        return;
    }

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
    if (isTeacherMode() || isAdminMode()) {
        ui->stackedWidget->setCurrentWidget(m_teacherAnalyticsPage);
        setActiveSection(ui->resultsButton);
        setHeader(
            "Аналитика",
            isAdminMode()
                ? "Выбирай любой курс системы и смотри попытки, средний результат и активность по тестам."
                : "Выбирай курс преподавателя и смотри студентов, попытки и средний результат по тестам.");
        if (m_courses.isEmpty()) {
            m_teacherAnalyticsPage->showMessage(
                isAdminMode()
                    ? "В системе пока нет курсов, поэтому аналитика ещё пуста."
                    : "Сначала нужен хотя бы один курс преподавателя.",
                false);
        } else if (m_teacherAnalyticsPage->selectedCourseId() >= 0) {
            loadTeacherAnalytics(m_teacherAnalyticsPage->selectedCourseId());
        }
        showStatus(isAdminMode() ? "Открыта admin-страница аналитики" : "Открыта teacher-страница аналитики");
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
    setHeader(
        "Профиль",
        isAdminMode()
            ? "Сводка по административной сессии и системному режиму доступа."
            : "Основные данные активной сессии пользователя.");
    showStatus("Личный кабинет открыт");
}

void MainWindow::onLogoutClicked()
{
    m_apiClient->setToken(QString());
    m_session = SessionData{};
    m_adminOverview = AdminOverviewData{};
    m_courses.clear();
    m_attempts.clear();
    m_selectedLessons.clear();
    m_selectedMaterials.clear();
    m_selectedTests.clear();
    m_selectedQuestions.clear();
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
    m_selectedQuestions.clear();
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
    if (!isTeacherMode() && !isAdminMode()) {
        onCourseOpened(course);
        return;
    }

    m_selectedCourse = course;
    m_selectedTest = TestData{};
    m_selectedLessons.clear();
    m_selectedMaterials.clear();
    m_selectedTests.clear();
    m_selectedQuestions.clear();
    refreshSelectedCourseFromCache();
    m_teacherCourseBuilderPage->setCourse(m_selectedCourse);
    m_teacherTestEditorPage->setCourse(m_selectedCourse);
    m_teacherCourseBuilderPage->setLessons({});
    m_teacherCourseBuilderPage->setMaterials({});
    m_teacherCourseBuilderPage->setTests({});
    m_teacherCourseBuilderPage->setQuestions({});
    m_teacherTestEditorPage->setTests({});
    m_teacherTestEditorPage->setQuestions({});
    m_teacherCourseBuilderPage->showMessage("Загружаем структуру курса...", false);
    showTeacherCourseBuilderPage();
    showStatus(QString("Открываем конструктор курса \"%1\"").arg(m_selectedCourse.title));
    loadCourseContent(m_selectedCourse.id);
}

void MainWindow::onEnrollRequested(int courseId)
{
    if (isTeacherMode() || isAdminMode()) {
        showStatus(isAdminMode()
            ? "В admin-режиме запись на курс недоступна. Здесь администратор управляет системой, а не записывается на обучение."
            : "В teacher-режиме запись на курс недоступна. Следующим шагом добавим управление курсами.");
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
            normalizedResult.testTitle = m_selectedTest.title;
            normalizedResult.submittedAt = QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm");
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
    } else if (isAdminMode()) {
        ui->stackedWidget->setCurrentWidget(m_courseDetailsPage);
        setActiveSection(ui->coursesButton);
        setHeader("Курс системы", "Административный обзор выбранного курса: структура, материалы, видео и тесты.");
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
    setHeader(
        "Конструктор курса",
        isAdminMode()
            ? "Административный конструктор курса: уроки, материалы и тесты с системным уровнем доступа."
            : "Собирай курс по шагам: уроки, материалы и тесты внутри teacher-кабинета.");
}

void MainWindow::showTeacherTestEditorPage()
{
    ui->stackedWidget->setCurrentWidget(m_teacherTestEditorPage);
    setActiveSection(ui->testButton);
    setHeader(
        "Редактор тестов",
        isAdminMode()
            ? "Отдельный системный экран для управления тестами, вопросами и вариантами ответов."
            : "Отдельное рабочее пространство преподавателя для тестов, вопросов и структуры ответов.");
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
            m_profilePage->setCourses(m_courses);

            if (m_courses.isEmpty()) {
                if (isTeacherMode()) {
                    m_teacherStudentsPage->setCourses({});
                }
                if (isTeacherMode() || isAdminMode()) {
                    m_teacherAnalyticsPage->setCourses({});
                }
                m_coursesPage->showPlaceholder(
                    isTeacherMode() ? "У преподавателя пока нет курсов" : isAdminMode() ? "В системе пока нет курсов" : "Курсов пока нет",
                    isTeacherMode()
                        ? "Создай первый курс во вкладке \"Создать курс\", и он сразу появится здесь."
                        : isAdminMode()
                            ? "Когда в системе появятся курсы, администратор увидит их здесь вместе с общей аналитикой."
                        : "Сервер пока не вернул ни одного курса. Позже здесь появятся учебные карточки.");
                showStatus("Каталог курсов пока пуст");
                return;
            }

            m_coursesPage->setCourses(m_courses);
            if (isTeacherMode()) {
                m_teacherStudentsPage->setCourses(m_courses);
            }
            if (isTeacherMode() || isAdminMode()) {
                m_teacherAnalyticsPage->setCourses(m_courses);
            }
            if (isAdminMode()) {
                loadAdminOverview();
            }
            refreshSelectedCourseFromCache();

            if (m_selectedCourse.id != kInvalidId) {
                if (isTeacherMode()) {
                    m_teacherCourseBuilderPage->setCourse(m_selectedCourse);
                    m_teacherTestEditorPage->setCourse(m_selectedCourse);
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
            if (isTeacherMode() || isAdminMode()) {
                m_teacherAnalyticsPage->clearAnalytics();
                m_teacherAnalyticsPage->showMessage(
                    isAdminMode() ? "Не удалось загрузить курсы системы." : "Не удалось загрузить курсы преподавателя.",
                    true);
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
    } else if (isAdminMode()) {
        m_attempts.clear();
        m_attemptsPage->showPlaceholder(
            "Admin использует отдельные страницы пользователей и аналитики. Student-история попыток здесь не применяется.");
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
            m_profilePage->setAttempts(attempts);
        },
        [this](const QString &error) {
            m_attemptsPage->showError(error);
            showStatus(error);
            qDebug() << "Load attempts error:" << error;
        });
}

void MainWindow::loadAdminUsers()
{
    if (!isAdminMode()) {
        return;
    }

    m_adminUsersPage->showMessage("Загружаем пользователей системы...", false);
    showStatus("Загружаем пользователей...");

    m_apiClient->getAdminUsers(
        this,
        [this](const QVector<AdminUserData> &users) {
            QVector<AdminUserData> visibleUsers = users;
            for (auto &user : visibleUsers) {
                user.editable = user.id != m_session.userId;
            }

            m_adminUsersPage->setUsers(visibleUsers);
            loadAdminOverview();
            m_adminUsersPage->showMessage(QString("Найдено пользователей: %1").arg(visibleUsers.size()), false);
            showStatus("Список пользователей обновлён");
        },
        [this](const QString &error) {
            m_adminUsersPage->clearUsers();
            m_adminUsersPage->showMessage(error, true);
            showStatus(error);
            qDebug() << "Load admin users error:" << error;
        });
}

void MainWindow::loadAdminOverview()
{
    if (!isAdminMode()) {
        return;
    }

    m_apiClient->getAdminOverview(
        this,
        [this](const AdminOverviewData &overview) {
            m_adminOverview = overview;
            m_dashboardPage->setAdminOverview(overview);
        },
        [this](const QString &error) {
            showStatus(error);
            qDebug() << "Load admin overview error:" << error;
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

void MainWindow::loadManagedQuestions(int testId)
{
    if ((!isTeacherMode() && !isAdminMode()) || testId < 0) {
        return;
    }

    m_teacherCourseBuilderPage->showMessage("Загружаем вопросы выбранного теста...", false);

    m_apiClient->getManageQuestions(
        testId,
        this,
        [this, testId](const QVector<QuestionData> &questions) {
            const bool testStillVisible = std::any_of(
                m_selectedTests.begin(),
                m_selectedTests.end(),
                [testId](const TestData &test) { return test.id == testId; });

            if (!testStillVisible) {
                return;
            }

            m_selectedQuestions = questions;
            m_teacherCourseBuilderPage->setQuestions(questions);
            m_teacherTestEditorPage->setQuestions(questions);

            if (questions.isEmpty()) {
                showStatus("У выбранного теста пока нет вопросов");
            } else {
                showStatus("Вопросы теста загружены");
            }
        },
        [this](const QString &error) {
            m_selectedQuestions.clear();
            m_teacherCourseBuilderPage->setQuestions({});
            m_teacherTestEditorPage->setQuestions({});
            m_teacherCourseBuilderPage->showMessage(error, true);
            showStatus(error);
            qDebug() << "Load managed questions error:" << error;
        });
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
            if (isTeacherMode() || isAdminMode()) {
                m_teacherCourseBuilderPage->setLessons(lessons);
            } else {
                m_courseDetailsPage->setLessons(lessons);
            }

            if (lessons.isEmpty()) {
                m_selectedMaterials.clear();
                if (isTeacherMode() || isAdminMode()) {
                    m_teacherCourseBuilderPage->setMaterials({});
                } else {
                    m_courseDetailsPage->setMaterials({}, {});
                }
                return;
            }

            m_apiClient->getCourseMaterials(
                courseId,
                this,
                [this, courseId](const QVector<MaterialData> &items) {
                    if (m_selectedCourse.id != courseId) {
                        return;
                    }

                    QVector<MaterialData> materials;
                    QVector<MaterialData> videos;
                    for (const MaterialData &item : items) {
                        if (item.type.compare("video", Qt::CaseInsensitive) == 0) {
                            videos.push_back(item);
                        } else {
                            materials.push_back(item);
                        }
                    }

                    m_selectedMaterials = materials;
                    for (const MaterialData &video : std::as_const(videos)) {
                        m_selectedMaterials.push_back(video);
                    }

                    if (isTeacherMode() || isAdminMode()) {
                        m_teacherCourseBuilderPage->setMaterials(m_selectedMaterials);
                    } else {
                        m_courseDetailsPage->setMaterials(materials, videos);
                    }

                    showStatus("Материалы курса загружены");
                },
                [this, courseId](const QString &error) {
                    if (m_selectedCourse.id != courseId) {
                        return;
                    }

                    m_selectedMaterials.clear();
                    if (isTeacherMode() || isAdminMode()) {
                        m_teacherCourseBuilderPage->setMaterials({});
                    } else {
                        m_courseDetailsPage->setMaterials({}, {});
                    }

                    showStatus(error);
                    qDebug() << "Load course materials error:" << error;
                });
        },
        [this, courseId](const QString &error) {
            if (m_selectedCourse.id != courseId) {
                return;
            }

            m_selectedLessons.clear();
            m_selectedMaterials.clear();
            if (isTeacherMode() || isAdminMode()) {
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
            if (isTeacherMode() || isAdminMode()) {
                m_teacherCourseBuilderPage->setTests(tests);
                m_teacherTestEditorPage->setTests(tests);
                m_selectedQuestions.clear();
                m_teacherCourseBuilderPage->setQuestions({});
                m_teacherTestEditorPage->setQuestions({});
            } else {
                m_courseDetailsPage->setTests(tests);
            }
            if (tests.isEmpty()) {
                showStatus("Для этого курса тестов пока нет");
                return;
            }

            if (isTeacherMode() || isAdminMode()) {
                loadManagedQuestions(tests.first().id);
            }

            showStatus("Тесты курса загружены");
        },
        [this, courseId](const QString &error) {
            if (m_selectedCourse.id != courseId) {
                return;
            }

            m_selectedTests.clear();
            if (isTeacherMode() || isAdminMode()) {
                m_teacherCourseBuilderPage->setTests({});
                m_teacherCourseBuilderPage->setQuestions({});
                m_teacherTestEditorPage->setTests({});
                m_teacherTestEditorPage->setQuestions({});
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

void MainWindow::loadTeacherStudentAttempts(int courseId, int studentId, const QString &studentLogin)
{
    if (!isTeacherMode()) {
        return;
    }

    m_teacherStudentsPage->showMessage(QString("Загружаем попытки студента %1...").arg(studentLogin), false);
    showStatus("Загружаем попытки студента...");

    m_apiClient->getTeacherStudentAttempts(
        courseId,
        studentId,
        this,
        [this, studentLogin](const QVector<TeacherStudentAttemptData> &attempts) {
            m_teacherStudentsPage->setStudentAttempts(studentLogin, attempts);
            showStatus("Попытки выбранного студента загружены");
        },
        [this](const QString &error) {
            m_teacherStudentsPage->clearStudentAttempts();
            m_teacherStudentsPage->showMessage(error, true);
            showStatus(error);
            qDebug() << "Load teacher student attempts error:" << error;
        });
}

void MainWindow::loadTeacherAnalytics(int courseId)
{
    if (!isTeacherMode() && !isAdminMode()) {
        return;
    }

    m_teacherAnalyticsPage->showMessage("Загружаем аналитику курса...", false);
    showStatus("Загружаем аналитику...");

    m_apiClient->getCourseAnalytics(
        courseId,
        this,
        [this](const TeacherCourseAnalyticsData &analytics) {
            m_teacherAnalyticsPage->setAnalytics(analytics);
            showStatus("Аналитика курса обновлена");
        },
        [this](const QString &error) {
            m_teacherAnalyticsPage->clearAnalytics();
            m_teacherAnalyticsPage->showMessage(error, true);
            showStatus(error);
            qDebug() << "Load teacher analytics error:" << error;
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

bool MainWindow::isAdminMode() const
{
    return m_session.role == "Admin";
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
    m_teacherAnalyticsPage->setRoleMode(m_session.role);

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
    } else if (isAdminMode()) {
        ui->brandLabel->setText("LMS Admin");
        ui->brandCaptionLabel->setText("Системное пространство");
        ui->homeButton->setText("Панель");
        ui->coursesButton->setText("Курсы");
        m_createCourseButton->hide();
        ui->testButton->setText("Пользователи");
        ui->resultsButton->setText("Аналитика");
        m_studentsButton->hide();
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
