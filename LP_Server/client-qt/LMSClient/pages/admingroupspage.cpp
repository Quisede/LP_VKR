#include "admingroupspage.h"
#include "../ui/uistyles.h"

#include <QAbstractItemView>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {
QString fullName(const AdminUserData &user)
{
    const QString name = QString("%1 %2").arg(user.lastName, user.firstName).trimmed();
    return name.isEmpty() ? user.login : name;
}

QString inputStyle()
{
    return
        "QLineEdit {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " font-size: 14px;"
        "}";
}
}

AdminGroupsPage::AdminGroupsPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("pageCard");
    auto *pageLayout = new QVBoxLayout(pageCard);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->setSpacing(16);

    auto *titleLabel = new QLabel("Учебные группы", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Здесь администратор управляет учебными группами и сразу видит студентов выбранной группы.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    m_messageLabel = new QLabel("Загрузите группы, чтобы начать управление составом.", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);

    auto *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(16);

    auto *groupsCard = new QFrame(pageCard);
    groupsCard->setObjectName("moduleCard");
    groupsCard->setMinimumWidth(360);
    auto *groupsLayout = new QVBoxLayout(groupsCard);
    groupsLayout->setContentsMargins(18, 18, 18, 18);
    groupsLayout->setSpacing(12);

    auto *groupsTitleLabel = new QLabel("Список групп", groupsCard);
    groupsTitleLabel->setObjectName("moduleTitleLabel");

    m_summaryLabel = new QLabel(groupsCard);
    m_summaryLabel->setObjectName("sectionHintLabel");
    m_summaryLabel->setWordWrap(true);

    m_groupsList = new QListWidget(groupsCard);
    m_groupsList->setSpacing(10);
    m_groupsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_groupsList->setFocusPolicy(Qt::NoFocus);
    m_groupsList->setStyleSheet(
        "QListWidget { background: transparent; border: none; outline: none; }"
        "QListWidget::item { background: #ffffff; border: 1px solid #dbe4f0; border-radius: 14px; padding: 12px; margin: 0; color: #0f172a; font-weight: 700; }"
        "QListWidget::item:selected { background: #dbeafe; border-color: #2563eb; color: #0f172a; }");

    m_newGroupEdit = new QLineEdit(groupsCard);
    m_newGroupEdit->setPlaceholderText("Новая группа, например ИВТ-403");
    m_newGroupEdit->setStyleSheet(inputStyle());
    m_newGroupEdit->setMinimumHeight(44);

    m_createButton = new QPushButton("Создать группу", groupsCard);
    m_createButton->setObjectName("cardAccentButton");
    m_createButton->setMinimumHeight(44);

    groupsLayout->addWidget(groupsTitleLabel);
    groupsLayout->addWidget(m_summaryLabel);
    groupsLayout->addWidget(m_groupsList, 1);
    groupsLayout->addWidget(m_newGroupEdit);
    groupsLayout->addWidget(m_createButton);

    auto *studentsCard = new QFrame(pageCard);
    studentsCard->setObjectName("moduleCard");
    auto *studentsLayout = new QVBoxLayout(studentsCard);
    studentsLayout->setContentsMargins(18, 18, 18, 18);
    studentsLayout->setSpacing(12);

    m_selectedGroupLabel = new QLabel("Группа не выбрана", studentsCard);
    m_selectedGroupLabel->setObjectName("moduleTitleLabel");

    m_renameGroupEdit = new QLineEdit(studentsCard);
    m_renameGroupEdit->setPlaceholderText("Новое название выбранной группы");
    m_renameGroupEdit->setStyleSheet(inputStyle());
    m_renameGroupEdit->setMinimumHeight(44);

    auto *actionsLayout = new QHBoxLayout();
    actionsLayout->setSpacing(10);
    m_renameButton = new QPushButton("Переименовать", studentsCard);
    m_renameButton->setObjectName("cardGhostButton");
    m_deleteButton = new QPushButton("Удалить группу", studentsCard);
    m_deleteButton->setObjectName("cardDangerButton");
    m_renameButton->setMinimumHeight(44);
    m_deleteButton->setMinimumHeight(44);
    actionsLayout->addWidget(m_renameButton);
    actionsLayout->addWidget(m_deleteButton);
    actionsLayout->addStretch();

    m_studentsTable = new QTableWidget(studentsCard);
    m_studentsTable->setColumnCount(5);
    m_studentsTable->setHorizontalHeaderLabels({"ID", "Логин", "ФИО", "Почта", "Телефон"});
    m_studentsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_studentsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_studentsTable->verticalHeader()->setVisible(false);
    m_studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_studentsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_studentsTable->setFocusPolicy(Qt::NoFocus);
    m_studentsTable->setAlternatingRowColors(true);
    m_studentsTable->setMinimumHeight(420);
    m_studentsTable->setStyleSheet(
        "QTableWidget { background-color: #ffffff; alternate-background-color: #f8fbff; color: #0f172a; border: 1px solid #dbe4f0; border-radius: 16px; gridline-color: #e2e8f0; }"
        "QHeaderView::section { background-color: #eff6ff; color: #1e293b; border: none; border-bottom: 1px solid #dbe4f0; padding: 10px 12px; font-weight: 700; }");

    studentsLayout->addWidget(m_selectedGroupLabel);
    studentsLayout->addWidget(m_renameGroupEdit);
    studentsLayout->addLayout(actionsLayout);
    studentsLayout->addWidget(m_studentsTable);

    columnsLayout->addWidget(groupsCard, 1);
    columnsLayout->addWidget(studentsCard, 2);

    pageLayout->addWidget(titleLabel);
    pageLayout->addWidget(hintLabel);
    pageLayout->addWidget(m_messageLabel);
    pageLayout->addLayout(columnsLayout);
    rootLayout->addWidget(pageCard);

    connect(m_createButton, &QPushButton::clicked, this, [this]() {
        emit createGroupRequested(m_newGroupEdit->text().trimmed());
    });
    connect(m_renameButton, &QPushButton::clicked, this, [this]() {
        emit renameGroupRequested(selectedGroup(), m_renameGroupEdit->text().trimmed());
    });
    connect(m_deleteButton, &QPushButton::clicked, this, [this]() {
        emit deleteGroupRequested(selectedGroup());
    });
    connect(m_groupsList, &QListWidget::itemSelectionChanged, this, [this]() {
        m_currentGroup = selectedGroup();
        m_renameGroupEdit->setText(m_currentGroup);
        refreshStudents();
    });

    refreshGroups();
    refreshStudents();
}

void AdminGroupsPage::setGroups(const QStringList &groups)
{
    m_groups = groups;
    refreshGroups();
}

void AdminGroupsPage::setUsers(const QVector<AdminUserData> &users)
{
    m_users = users;
    refreshStudents();
}

void AdminGroupsPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(error
        ? "color: #b91c1c; font-size: 13px; font-weight: 600;"
        : "color: #0f766e; font-size: 13px; font-weight: 600;");
}

void AdminGroupsPage::setBusy(bool busy)
{
    m_newGroupEdit->setEnabled(!busy);
    m_renameGroupEdit->setEnabled(!busy && !selectedGroup().isEmpty());
    m_groupsList->setEnabled(!busy);
    m_createButton->setEnabled(!busy);
    m_renameButton->setEnabled(!busy && !selectedGroup().isEmpty());
    m_deleteButton->setEnabled(!busy && !selectedGroup().isEmpty());
}

QString AdminGroupsPage::selectedGroup() const
{
    auto *item = m_groupsList->currentItem();
    return item == nullptr ? QString() : item->data(Qt::UserRole).toString();
}

void AdminGroupsPage::refreshGroups()
{
    const QString previous = selectedGroup().isEmpty() ? m_currentGroup : selectedGroup();
    m_groupsList->clear();

    for (const QString &group : m_groups) {
        auto *item = new QListWidgetItem(group);
        item->setData(Qt::UserRole, group);
        m_groupsList->addItem(item);
        if (group == previous) {
            m_groupsList->setCurrentItem(item);
        }
    }

    if (m_groupsList->currentItem() == nullptr && m_groupsList->count() > 0) {
        m_groupsList->setCurrentRow(0);
    }

    m_currentGroup = selectedGroup();
    m_summaryLabel->setText(m_groups.isEmpty()
        ? "Учебные группы пока не созданы."
        : QString("Всего учебных групп: %1").arg(m_groups.size()));
    refreshStudents();
}

void AdminGroupsPage::refreshStudents()
{
    const QString group = selectedGroup();
    QVector<AdminUserData> students;
    for (const AdminUserData &user : m_users) {
        if (user.role == "Student" && user.groupName == group) {
            students.push_back(user);
        }
    }

    m_selectedGroupLabel->setText(group.isEmpty()
        ? "Группа не выбрана"
        : QString("%1: студентов %2").arg(group).arg(students.size()));

    m_studentsTable->clearContents();
    m_studentsTable->setRowCount(students.size());
    for (int row = 0; row < students.size(); ++row) {
        const auto &student = students[row];
        m_studentsTable->setItem(row, 0, new QTableWidgetItem(QString::number(student.id)));
        m_studentsTable->setItem(row, 1, new QTableWidgetItem(student.login));
        m_studentsTable->setItem(row, 2, new QTableWidgetItem(fullName(student)));
        m_studentsTable->setItem(row, 3, new QTableWidgetItem(student.email.isEmpty() ? "—" : student.email));
        m_studentsTable->setItem(row, 4, new QTableWidgetItem(student.phone.isEmpty() ? "—" : student.phone));
    }
    m_studentsTable->resizeRowsToContents();
    setBusy(false);
}
