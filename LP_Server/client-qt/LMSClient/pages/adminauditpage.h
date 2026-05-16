#pragma once

#include <QWidget>
#include <QVector>

#include "../models/adminauditmodel.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QTableWidget;

class AdminAuditPage : public QWidget
{
    Q_OBJECT

public:
    explicit AdminAuditPage(QWidget *parent = nullptr);

    void setEvents(const QVector<AdminAuditEventData> &events);
    void clearEvents();
    void showMessage(const QString &message, bool error = false);

private:
    void applyFilter();
    QString actionCaption(const QString &action) const;

    QVector<AdminAuditEventData> m_allEvents;
    QLabel *m_messageLabel;
    QLabel *m_totalValueLabel;
    QLabel *m_userEventsValueLabel;
    QLabel *m_latestActorValueLabel;
    QLabel *m_emptyStateLabel;
    QLineEdit *m_searchEdit;
    QComboBox *m_actionFilterCombo;
    QTableWidget *m_eventsTable;
};
