#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void setToken(const QString& token);

private slots:
    void onLoadCoursesClicked();
    void onEnrollClicked();
    void onLoadTestClicked();
    void onSubmitTestClicked();
    void showCoursesPage();
    void showTestPage();

private:
    int currentCourseId() const;

    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    QString authToken;

    int currentTestId = -1;
    int currentQuestionId = -1;
};
#endif // MAINWINDOW_H
