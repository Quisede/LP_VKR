#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QNetworkAccessManager>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

signals:
    // сигнал при успешном логине
    void loginSuccess(QString token);

private slots:
    void onLoginClicked();

private:
    Ui::LoginWindow *ui;
    QNetworkAccessManager* networkManager;

};

#endif // LOGINWINDOW_H
