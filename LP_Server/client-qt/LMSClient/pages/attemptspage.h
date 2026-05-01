#pragma once

#include <QWidget>
#include <QVector>

#include "../models/attemptmodel.h"

class QLabel;
class QTableWidget;

class AttemptsPage : public QWidget
{
    Q_OBJECT

public:
    explicit AttemptsPage(QWidget *parent = nullptr);

    void setRoleMode(const QString &role);
    void setAttempts(const QVector<AttemptData> &attempts);
    void showPlaceholder(const QString &message);
    void showError(const QString &error);

private:
    QString m_role = "Student";
    QLabel *m_titleLabel;
    QLabel *m_summaryLabel;
    QTableWidget *m_table;
};
