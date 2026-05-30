#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

#include "../models/sessiondata.h"

namespace Ui {
class LoginWindow;
}

class ApiClient;

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(ApiClient *apiClient, QWidget *parent = nullptr);
    ~LoginWindow();
    void resetState();

signals:
    void loginSuccess(const SessionData &session);

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    Ui::LoginWindow *ui;
    ApiClient *m_apiClient;
};

#endif // LOGINWINDOW_H
