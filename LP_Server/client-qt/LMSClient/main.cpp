#include <QApplication>
#include "loginwindow.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoginWindow login;
    login.show();

    QObject::connect(&login, &LoginWindow::loginSuccess,
                     [&login](const QString &token) {
        MainWindow *mainWin = new MainWindow();
        mainWin->setAttribute(Qt::WA_DeleteOnClose);
        mainWin->setToken(token);
        mainWin->show();
        login.close();
    });

    return a.exec();
}
