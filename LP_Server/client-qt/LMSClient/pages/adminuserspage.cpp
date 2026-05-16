#include "adminuserspage.h"
#include "../ui/uistyles.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

namespace {

QFrame *createStatCard(const QString &title, QLabel **valueLabel, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("moduleCard");
    card->setMinimumHeight(136);
    card->setMaximumHeight(156);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("moduleTitleLabel");

    *valueLabel = new QLabel("0", card);
    (*valueLabel)->setObjectName("courseDetailTitleLabel");

    layout->addWidget(titleLabel);
    layout->addWidget(*valueLabel);
    layout->addStretch();
    return card;
}

QString roleCaption(const QString &role)
{
    if (role == "Student") return "Студент";
    if (role == "Teacher") return "Преподаватель";
    if (role == "Admin") return "Администратор";
    return role;
}

}

AdminUsersPage::AdminUsersPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("pageCard");

    auto *layout = new QVBoxLayout(pageCard);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    auto *titleLabel = new QLabel("Пользователи системы", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Здесь собран реестр пользователей платформы. Эта страница служит стартовой точкой для admin-flow.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    m_messageLabel = new QLabel("Загрузи список пользователей, чтобы увидеть общую картину по ролям.", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);

    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);
    statsLayout->addWidget(createStatCard("Всего пользователей", &m_totalUsersValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("Студенты", &m_studentsValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("Преподаватели", &m_teachersValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("Администраторы", &m_adminsValueLabel, pageCard));

    const QString inputStyle =
        "QLineEdit {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " font-size: 14px;"
        "}";

    auto *formCard = new QFrame(pageCard);
    formCard->setObjectName("moduleCard");
    auto *formLayout = new QVBoxLayout(formCard);
    formLayout->setContentsMargins(18, 18, 18, 18);
    formLayout->setSpacing(12);

    auto *formTitle = new QLabel("Создать пользователя", formCard);
    formTitle->setObjectName("moduleTitleLabel");

    auto *formHint = new QLabel(
        "Администратор может сразу создать студента, преподавателя или другого администратора.",
        formCard);
    formHint->setObjectName("sectionHintLabel");
    formHint->setWordWrap(true);

    m_loginEdit = new QLineEdit(formCard);
    m_loginEdit->setPlaceholderText("Логин");
    m_loginEdit->setStyleSheet(inputStyle);

    m_passwordEdit = new QLineEdit(formCard);
    m_passwordEdit->setPlaceholderText("Пароль");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setStyleSheet(inputStyle);

    m_roleCombo = ui_styles::createComboBox(formCard);
    m_roleCombo->addItems({"Student", "Teacher", "Admin"});
    ui_styles::applyComboBoxStyle(m_roleCombo);

    m_createButton = new QPushButton("Создать пользователя", formCard);
    m_createButton->setObjectName("cardAccentButton");

    formLayout->addWidget(formTitle);
    formLayout->addWidget(formHint);
    formLayout->addWidget(m_loginEdit);
    formLayout->addWidget(m_passwordEdit);
    formLayout->addWidget(m_roleCombo);
    formLayout->addWidget(m_createButton, 0, Qt::AlignLeft);

    m_focusLabel = new QLabel(pageCard);
    m_focusLabel->setObjectName("sectionHintLabel");
    m_focusLabel->setWordWrap(true);

    m_searchEdit = new QLineEdit(pageCard);
    m_searchEdit->setPlaceholderText("Найти пользователя по логину или роли...");
    m_searchEdit->setStyleSheet(inputStyle);
    connect(m_searchEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        applyUsersFilter(text);
    });

    m_filterRoleCombo = ui_styles::createComboBox(pageCard);
    m_filterRoleCombo->addItems({"Все роли", "Student", "Teacher", "Admin"});
    ui_styles::applyComboBoxStyle(m_filterRoleCombo);
    m_filterRoleCombo->setMinimumWidth(180);
    connect(m_filterRoleCombo, &QComboBox::currentTextChanged, this, [this]() {
        applyUsersFilter(m_searchEdit->text());
    });

    auto *searchRowLayout = new QHBoxLayout();
    searchRowLayout->setSpacing(10);
    searchRowLayout->addWidget(m_searchEdit, 1);
    searchRowLayout->addWidget(m_filterRoleCombo, 0);

    m_emptyStateLabel = new QLabel(pageCard);
    m_emptyStateLabel->setObjectName("sectionHintLabel");
    m_emptyStateLabel->setWordWrap(true);

    m_usersTable = new QTableWidget(pageCard);
    m_usersTable->setColumnCount(4);
    m_usersTable->setHorizontalHeaderLabels({"ID", "Логин", "Роль", "Управление"});
    m_usersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_usersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_usersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_usersTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_usersTable->verticalHeader()->setVisible(false);
    m_usersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_usersTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_usersTable->setFocusPolicy(Qt::NoFocus);
    m_usersTable->setAlternatingRowColors(true);
    m_usersTable->verticalHeader()->setDefaultSectionSize(66);
    m_usersTable->setMinimumHeight(360);
    m_usersTable->setStyleSheet(
        "QTableWidget {"
        " background-color: #ffffff;"
        " alternate-background-color: #f8fbff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 16px;"
        " gridline-color: #e2e8f0;"
        "}"
        "QHeaderView::section {"
        " background-color: #eff6ff;"
        " color: #1e293b;"
        " border: none;"
        " border-bottom: 1px solid #dbe4f0;"
        " padding: 10px 12px;"
        " font-weight: 700;"
        "}");

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(m_messageLabel);
    layout->addLayout(statsLayout);
    layout->addWidget(formCard);
    layout->addWidget(m_focusLabel);
    layout->addLayout(searchRowLayout);
    layout->addWidget(m_emptyStateLabel);
    layout->addWidget(m_usersTable);
    rootLayout->addWidget(pageCard);

    connect(m_createButton, &QPushButton::clicked, this, [this]() {
        emit createUserRequested(
            m_loginEdit->text().trimmed(),
            m_passwordEdit->text(),
            m_roleCombo->currentText());
    });

    clearUsers();
}

void AdminUsersPage::setUsers(const QVector<AdminUserData> &users)
{
    m_allUsers = users;
    applyUsersFilter(m_searchEdit->text());
}

void AdminUsersPage::applyUsersFilter(const QString &query)
{
    int students = 0;
    int teachers = 0;
    int admins = 0;
    const QString normalizedQuery = query.trimmed().toLower();
    const QString selectedRole = m_filterRoleCombo ? m_filterRoleCombo->currentText() : QString("Все роли");
    QVector<AdminUserData> users;

    for (const auto &user : m_allUsers) {
        const QString haystack = (user.login + " " + user.role).toLower();
        const bool roleMatches = selectedRole == "Все роли" || user.role == selectedRole;
        if ((normalizedQuery.isEmpty() || haystack.contains(normalizedQuery)) && roleMatches) {
            users.push_back(user);
        }
    }

    m_usersTable->clearContents();
    m_usersTable->setRowCount(users.size());

    for (int row = 0; row < users.size(); ++row) {
        const auto &user = users[row];

        if (user.role == "Student") ++students;
        if (user.role == "Teacher") ++teachers;
        if (user.role == "Admin") ++admins;

        auto *idItem = new QTableWidgetItem(QString::number(user.id));
        auto *loginItem = new QTableWidgetItem(user.login);
        auto *roleItem = new QTableWidgetItem(roleCaption(user.role));

        idItem->setTextAlignment(Qt::AlignCenter);
        roleItem->setTextAlignment(Qt::AlignCenter);
        if (user.role == "Admin") {
            roleItem->setForeground(QColor("#7c3aed"));
        } else if (user.role == "Teacher") {
            roleItem->setForeground(QColor("#2563eb"));
        } else {
            roleItem->setForeground(QColor("#15803d"));
        }

        auto *actionsWidget = new QWidget(m_usersTable);
        auto *actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(8, 4, 8, 4);
        actionsLayout->setSpacing(8);

        auto *roleCombo = ui_styles::createComboBox(actionsWidget);
        roleCombo->addItems({"Student", "Teacher", "Admin"});
        roleCombo->setCurrentText(user.role);
        ui_styles::applyComboBoxStyle(roleCombo);
        roleCombo->setMinimumWidth(128);
        roleCombo->setMaximumWidth(156);
        roleCombo->setEnabled(user.editable);

        auto *saveButton = new QPushButton("Сохранить", actionsWidget);
        saveButton->setObjectName("cardGhostButton");
        saveButton->setMinimumWidth(108);
        saveButton->setMaximumWidth(118);
        saveButton->setEnabled(user.editable);

        auto *deleteButton = new QPushButton("Удалить", actionsWidget);
        deleteButton->setObjectName("cardDangerButton");
        deleteButton->setMinimumWidth(92);
        deleteButton->setMaximumWidth(108);
        deleteButton->setEnabled(user.editable);

        actionsLayout->addWidget(roleCombo);
        actionsLayout->addWidget(saveButton);
        actionsLayout->addWidget(deleteButton);

        connect(saveButton, &QPushButton::clicked, this, [this, user, roleCombo]() {
            emit updateUserRoleRequested(user.id, roleCombo->currentText());
        });
        connect(deleteButton, &QPushButton::clicked, this, [this, user]() {
            emit deleteUserRequested(user.id);
        });

        m_usersTable->setItem(row, 0, idItem);
        m_usersTable->setItem(row, 1, loginItem);
        m_usersTable->setItem(row, 2, roleItem);
        m_usersTable->setCellWidget(row, 3, actionsWidget);
    }

    m_usersTable->resizeRowsToContents();

    m_totalUsersValueLabel->setText(QString::number(users.size()));
    m_studentsValueLabel->setText(QString::number(students));
    m_teachersValueLabel->setText(QString::number(teachers));
    m_adminsValueLabel->setText(QString::number(admins));

    if (users.isEmpty()) {
        if (m_allUsers.isEmpty()) {
            m_focusLabel->setText("Система пока не содержит пользователей. После регистрации или seed-инициализации они появятся здесь.");
            m_emptyStateLabel->setText("Таблица пока пуста: в системе ещё нет ни студентов, ни преподавателей.");
        } else {
            m_focusLabel->setText("По текущему запросу пользователи не найдены.");
            m_emptyStateLabel->setText("Можно изменить строку поиска: поиск работает по логину или роли.");
        }
        m_emptyStateLabel->show();
        m_usersTable->hide();
    } else {
        m_focusLabel->setText(
            QString("Сейчас в системе %1 пользователей: %2 студентов, %3 преподавателей и %4 администраторов.")
                .arg(users.size())
                .arg(students)
                .arg(teachers)
                .arg(admins));
        m_emptyStateLabel->hide();
        m_usersTable->show();
    }
}

void AdminUsersPage::clearUsers()
{
    m_allUsers.clear();
    m_totalUsersValueLabel->setText("0");
    m_studentsValueLabel->setText("0");
    m_teachersValueLabel->setText("0");
    m_adminsValueLabel->setText("0");
    m_focusLabel->setText("Как только список пользователей загрузится, здесь появится общая срезка по ролям.");
    m_emptyStateLabel->setText("После загрузки здесь появится таблица пользователей системы.");
    m_emptyStateLabel->show();
    m_usersTable->hide();
    m_usersTable->setRowCount(0);
    if (m_filterRoleCombo) {
        m_filterRoleCombo->setCurrentIndex(0);
    }
}

void AdminUsersPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

void AdminUsersPage::setBusy(bool busy)
{
    m_loginEdit->setEnabled(!busy);
    m_passwordEdit->setEnabled(!busy);
    m_roleCombo->setEnabled(!busy);
    m_createButton->setEnabled(!busy);
}

void AdminUsersPage::clearDraft()
{
    m_loginEdit->clear();
    m_passwordEdit->clear();
    m_roleCombo->setCurrentIndex(0);
}
