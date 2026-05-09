#include <QApplication>
#include <QIcon>
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
