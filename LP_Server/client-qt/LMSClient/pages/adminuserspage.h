#pragma once

#include <QWidget>
#include <QStringList>
#include <QVector>

#include "../models/adminusermodel.h"

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QTableWidget;

class AdminUsersPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminUsersPage(QWidget *parent = nullptr);

    void setUsers(const QVector<AdminUserData> &users);
    void setGroups(const QStringList &groups);
    void clearUsers();
    void showMessage(const QString &message, bool error = false);
    void setBusy(bool busy);
    void clearDraft();

signals:
    void createUserRequested(
        const QString &login,
        const QString &password,
        const QString &role,
        const QString &firstName,
        const QString &lastName,
        const QString &groupName,
        const QString &email,
        const QString &phone);
    void createGroupRequested(const QString &groupName);
    void updateUserRoleRequested(int userId, const QString &role);
    void deleteUserRequested(int userId);

private:
    void applyUsersFilter(const QString &query = QString());

    QVector<AdminUserData> m_allUsers;
    QLabel *m_messageLabel;
    QLabel *m_totalUsersValueLabel;
    QLabel *m_studentsValueLabel;
    QLabel *m_teachersValueLabel;
    QLabel *m_adminsValueLabel;
    QLabel *m_focusLabel;
    QLabel *m_emptyStateLabel;
    QLineEdit *m_searchEdit;
    QComboBox *m_filterRoleCombo;
    QLineEdit *m_loginEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_firstNameEdit;
    QLineEdit *m_lastNameEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_phoneEdit;
    QLineEdit *m_newGroupEdit;
    QComboBox *m_roleCombo;
    QComboBox *m_groupCombo;
    QPushButton *m_createButton;
    QPushButton *m_createGroupButton;
    QTableWidget *m_usersTable;
};
