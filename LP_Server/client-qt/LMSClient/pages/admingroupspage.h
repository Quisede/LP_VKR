#pragma once

#include <QStringList>
#include <QVector>
#include <QWidget>

#include "../models/adminusermodel.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTableWidget;

class AdminGroupsPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminGroupsPage(QWidget *parent = nullptr);

    void setGroups(const QStringList &groups);
    void setUsers(const QVector<AdminUserData> &users);
    void showMessage(const QString &message, bool error = false);
    void setBusy(bool busy);
    QString selectedGroup() const;

signals:
    void createGroupRequested(const QString &groupName);
    void renameGroupRequested(const QString &oldName, const QString &newName);
    void deleteGroupRequested(const QString &groupName);

private:
    void refreshGroups();
    void refreshStudents();

    QStringList m_groups;
    QVector<AdminUserData> m_users;
    QString m_currentGroup;

    QLabel *m_messageLabel;
    QLabel *m_summaryLabel;
    QLabel *m_selectedGroupLabel;
    QLineEdit *m_newGroupEdit;
    QLineEdit *m_renameGroupEdit;
    QListWidget *m_groupsList;
    QTableWidget *m_studentsTable;
    QPushButton *m_createButton;
    QPushButton *m_renameButton;
    QPushButton *m_deleteButton;
};
