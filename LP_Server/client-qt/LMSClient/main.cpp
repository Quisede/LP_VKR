#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QTimer>
#include <QStyleFactory>
#include "api/apiclient.h"
#include "mainwindow.h"
#include "windows/loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));
    a.setWindowIcon(QIcon(":/icons/lms-app.svg"));
    a.setApplicationName("LMS Client");
    a.setApplicationDisplayName("LMS Client");

    qRegisterMetaType<SessionData>("SessionData");

    ApiClient apiClient;
    const QString baseUrl = qEnvironmentVariable("LMS_API_BASE_URL");
    if (!baseUrl.isEmpty()) {
        apiClient.setBaseUrl(baseUrl);
    }

    const bool automationMode = qEnvironmentVariableIntValue("LMS_AUTOMATION") == 1;
    if (automationMode) {
        const QString page = qEnvironmentVariable("LMS_AUTOMATION_PAGE", "dashboard");
        const QString screenshotPath = qEnvironmentVariable("LMS_AUTOMATION_SCREENSHOT_PATH");
        const QString courseTitle = qEnvironmentVariable("LMS_AUTOMATION_COURSE_TITLE");
        const QString login = qEnvironmentVariable("LMS_AUTOMATION_LOGIN");
        const QString password = qEnvironmentVariable("LMS_AUTOMATION_PASSWORD");
        const int openDelayMs = qEnvironmentVariableIntValue("LMS_AUTOMATION_OPEN_DELAY_MS") > 0
            ? qEnvironmentVariableIntValue("LMS_AUTOMATION_OPEN_DELAY_MS")
            : 1200;
        const int captureDelayMs = qEnvironmentVariableIntValue("LMS_AUTOMATION_CAPTURE_DELAY_MS") > 0
            ? qEnvironmentVariableIntValue("LMS_AUTOMATION_CAPTURE_DELAY_MS")
            : 3200;

        if (page == "login") {
            LoginWindow loginWindow(&apiClient);
            loginWindow.show();

            QTimer::singleShot(captureDelayMs, &loginWindow, [&a, &loginWindow, screenshotPath]() {
                if (!screenshotPath.isEmpty()) {
                    QDir().mkpath(QFileInfo(screenshotPath).absolutePath());
                    loginWindow.grab().save(screenshotPath);
                    qDebug() << "Saved automation screenshot:" << screenshotPath;
                }
                a.quit();
            });

            return a.exec();
        }

        if (login.isEmpty() || password.isEmpty()) {
            qCritical() << "Automation mode requires LMS_AUTOMATION_LOGIN and LMS_AUTOMATION_PASSWORD";
            return 2;
        }

        apiClient.login(
            login,
            password,
            &a,
            [&a, &apiClient, page, courseTitle, screenshotPath, openDelayMs, captureDelayMs](const SessionData &session) {
                auto *mainWin = new MainWindow(&apiClient);
                mainWin->setAttribute(Qt::WA_DeleteOnClose);
                mainWin->resize(1680, 1050);
                mainWin->setSession(session);
                mainWin->show();

                QTimer::singleShot(openDelayMs, mainWin, [mainWin, page, courseTitle]() {
                    mainWin->automationOpenPage(page, courseTitle);
                });

                QTimer::singleShot(captureDelayMs, mainWin, [&a, mainWin, screenshotPath]() {
                    if (!screenshotPath.isEmpty()) {
                        QDir().mkpath(QFileInfo(screenshotPath).absolutePath());
                        mainWin->grab().save(screenshotPath);
                        qDebug() << "Saved automation screenshot:" << screenshotPath;
                    }
                    mainWin->close();
                    a.quit();
                });
            },
            [&a](const QString &error) {
                qCritical() << "Automation login failed:" << error;
                a.exit(3);
            });

        return a.exec();
    }

    LoginWindow login(&apiClient);
    login.show();

    QObject::connect(&login, &LoginWindow::loginSuccess,
                     [&login, &apiClient](const SessionData &session) {
        MainWindow *mainWin = new MainWindow(&apiClient);
        mainWin->setAttribute(Qt::WA_DeleteOnClose);
        mainWin->setSession(session);

        QObject::connect(mainWin, &MainWindow::logoutRequested,
                         [&login, mainWin]() {
            login.resetState();
            login.show();
            login.raise();
            login.activateWindow();
            mainWin->close();
        });

        mainWin->show();
        login.close();
    });

    return a.exec();
}
