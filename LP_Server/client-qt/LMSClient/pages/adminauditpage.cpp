#include "adminauditpage.h"

#include "../ui/uistyles.h"

#include <QAbstractItemView>
#include <QColor>
#include <QComboBox>
#include <QFrame>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

QFrame *createStatCard(const QString &title, QLabel **valueLabel, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName("moduleCard");
    card->setMinimumHeight(132);
    card->setMaximumHeight(152);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("moduleTitleLabel");

    *valueLabel = new QLabel("0", card);
    (*valueLabel)->setObjectName("courseDetailTitleLabel");
    (*valueLabel)->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(*valueLabel);
    layout->addStretch();
    return card;
}

}

AdminAuditPage::AdminAuditPage(QWidget *parent)
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

    auto *titleLabel = new QLabel("Журнал действий", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto *hintLabel = new QLabel(
        "Здесь собраны административные события из backend-журнала: создание пользователей, смена ролей и удаление аккаунтов.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);
    hintLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_messageLabel = new QLabel("Загружаем события журнала...", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);
    statsLayout->addWidget(createStatCard("Событий", &m_totalValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("User-события", &m_userEventsValueLabel, pageCard));
    statsLayout->addWidget(createStatCard("Последний автор", &m_latestActorValueLabel, pageCard));

    const QString inputStyle =
        "QLineEdit {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " font-size: 14px;"
        "}";

    m_searchEdit = new QLineEdit(pageCard);
    m_searchEdit->setPlaceholderText("Найти по автору, действию или деталям...");
    m_searchEdit->setStyleSheet(inputStyle);

    m_actionFilterCombo = ui_styles::createComboBox(pageCard);
    m_actionFilterCombo->addItems({
        "Все действия",
        "Создание пользователей",
        "Смена ролей",
        "Удаление пользователей"
    });
    ui_styles::applyComboBoxStyle(m_actionFilterCombo);
    m_actionFilterCombo->setMinimumWidth(220);

    auto *filterLayout = new QHBoxLayout();
    filterLayout->setSpacing(10);
    filterLayout->addWidget(m_searchEdit, 1);
    filterLayout->addWidget(m_actionFilterCombo, 0);

    m_emptyStateLabel = new QLabel(pageCard);
    m_emptyStateLabel->setObjectName("sectionHintLabel");
    m_emptyStateLabel->setWordWrap(true);
    m_emptyStateLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_eventsTable = new QTableWidget(pageCard);
    m_eventsTable->setColumnCount(5);
    m_eventsTable->setHorizontalHeaderLabels({"Когда", "Автор", "Действие", "Объект", "Детали"});
    m_eventsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_eventsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_eventsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_eventsTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_eventsTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_eventsTable->verticalHeader()->setVisible(false);
    m_eventsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_eventsTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_eventsTable->setFocusPolicy(Qt::NoFocus);
    m_eventsTable->setAlternatingRowColors(true);
    m_eventsTable->setMinimumHeight(420);
    m_eventsTable->setStyleSheet(
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
        "}"
        "QTableWidget::item {"
        " padding: 9px;"
        " border-bottom: 1px solid #eef2f7;"
        "}");

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(m_messageLabel);
    layout->addLayout(statsLayout);
    layout->addLayout(filterLayout);
    layout->addWidget(m_emptyStateLabel);
    layout->addWidget(m_eventsTable);
    layout->addStretch();

    rootLayout->addWidget(pageCard);

    connect(m_searchEdit, &QLineEdit::textChanged, this, [this]() { applyFilter(); });
    connect(m_actionFilterCombo, &QComboBox::currentTextChanged, this, [this]() { applyFilter(); });

    clearEvents();
}

void AdminAuditPage::setEvents(const QVector<AdminAuditEventData> &events)
{
    m_allEvents = events;

    int userEvents = 0;
    for (const AdminAuditEventData &event : m_allEvents) {
        if (event.targetType == "user") {
            ++userEvents;
        }
    }

    m_totalValueLabel->setText(QString::number(m_allEvents.size()));
    m_userEventsValueLabel->setText(QString::number(userEvents));
    m_latestActorValueLabel->setText(
        m_allEvents.isEmpty()
            ? "—"
            : (m_allEvents.first().adminLogin.isEmpty() ? "admin" : m_allEvents.first().adminLogin));

    showMessage(
        m_allEvents.isEmpty()
            ? "Журнал пока пуст. Выполни admin-действие, чтобы появилась первая запись."
            : QString("Загружено событий: %1").arg(m_allEvents.size()),
        false);

    applyFilter();
}

void AdminAuditPage::clearEvents()
{
    m_allEvents.clear();
    m_totalValueLabel->setText("0");
    m_userEventsValueLabel->setText("0");
    m_latestActorValueLabel->setText("—");
    m_emptyStateLabel->setText("Журнал пока пуст. После admin-действий здесь появится история изменений.");
    m_emptyStateLabel->show();
    m_eventsTable->hide();
    m_eventsTable->setRowCount(0);
}

void AdminAuditPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

void AdminAuditPage::applyFilter()
{
    const QString query = m_searchEdit->text().trimmed().toLower();
    const QString actionFilter = m_actionFilterCombo->currentText();

    QVector<AdminAuditEventData> visibleEvents;
    for (const AdminAuditEventData &event : m_allEvents) {
        bool actionMatches = actionFilter == "Все действия";
        if (actionFilter == "Создание пользователей") {
            actionMatches = event.action == "user.created";
        } else if (actionFilter == "Смена ролей") {
            actionMatches = event.action == "user.role_updated";
        } else if (actionFilter == "Удаление пользователей") {
            actionMatches = event.action == "user.deleted";
        }

        const QString haystack = QString("%1 %2 %3 %4")
                                     .arg(event.adminLogin, event.action, event.targetType, event.details)
                                     .toLower();
        if (actionMatches && (query.isEmpty() || haystack.contains(query))) {
            visibleEvents.push_back(event);
        }
    }

    m_eventsTable->clearContents();
    m_eventsTable->setRowCount(visibleEvents.size());

    for (int row = 0; row < visibleEvents.size(); ++row) {
        const AdminAuditEventData &event = visibleEvents[row];

        auto *createdAtItem = new QTableWidgetItem(event.createdAt);
        auto *actorItem = new QTableWidgetItem(event.adminLogin.isEmpty() ? "admin" : event.adminLogin);
        auto *actionItem = new QTableWidgetItem(actionCaption(event.action));
        auto *targetItem = new QTableWidgetItem(
            event.targetId >= 0
                ? QString("%1 #%2").arg(event.targetType).arg(event.targetId)
                : event.targetType);
        auto *detailsItem = new QTableWidgetItem(event.details);

        actionItem->setForeground(event.action == "user.deleted" ? QColor("#b91c1c") : QColor("#2563eb"));

        m_eventsTable->setItem(row, 0, createdAtItem);
        m_eventsTable->setItem(row, 1, actorItem);
        m_eventsTable->setItem(row, 2, actionItem);
        m_eventsTable->setItem(row, 3, targetItem);
        m_eventsTable->setItem(row, 4, detailsItem);
    }

    if (visibleEvents.isEmpty()) {
        m_emptyStateLabel->setText(
            m_allEvents.isEmpty()
                ? "Журнал пока пуст. После создания, удаления или смены роли пользователя записи появятся здесь."
                : "По текущему фильтру событий не найдено.");
        m_emptyStateLabel->show();
        m_eventsTable->hide();
    } else {
        m_emptyStateLabel->hide();
        m_eventsTable->show();
        m_eventsTable->resizeRowsToContents();
    }
}

QString AdminAuditPage::actionCaption(const QString &action) const
{
    if (action == "user.created") {
        return "Создание пользователя";
    }
    if (action == "user.role_updated") {
        return "Смена роли";
    }
    if (action == "user.deleted") {
        return "Удаление пользователя";
    }
    return action;
}
