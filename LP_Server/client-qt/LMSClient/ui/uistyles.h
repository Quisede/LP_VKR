#pragma once

#include <QComboBox>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QListView>
#include <QScreen>
#include <QString>

namespace ui_styles {

inline QString comboBoxStyle()
{
    return
        "QComboBox {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 14px;"
        " font-size: 14px;"
        " min-height: 22px;"
        "}"
        "QComboBox:hover {"
        " border: 1px solid #93c5fd;"
        "}"
        "QComboBox:focus {"
        " border: 1px solid #2563eb;"
        "}"
        "QComboBox::drop-down {"
        " width: 34px;"
        " border: none;"
        " background: transparent;"
        "}";
}

inline QString comboPopupViewStyle()
{
    return
        "QListView {"
        " background: #ffffff;"
        " color: #0f172a;"
        " border: none;"
        " outline: none;"
        " padding: 6px;"
        "}"
        "QListView::item {"
        " min-height: 30px;"
        " padding: 6px 10px;"
        " border-radius: 10px;"
        "}"
        "QListView::item:hover {"
        " background: #eff6ff;"
        " color: #1d4ed8;"
        "}"
        "QListView::item:selected {"
        " background: #dbeafe;"
        " color: #1d4ed8;"
        "}";
}

class StyledComboBox final : public QComboBox
{
public:
    explicit StyledComboBox(QWidget *parent = nullptr)
        : QComboBox(parent)
    {
        setStyleSheet(comboBoxStyle());
    }

protected:
    void showPopup() override
    {
        ensurePopup();

        if (!m_popupFrame || !m_popupView) {
            QComboBox::showPopup();
            return;
        }

        if (m_popupFrame->isVisible()) {
            hidePopup();
            return;
        }

        m_popupView->setModel(model());
        m_popupView->setModelColumn(modelColumn());
        m_popupView->setRootIndex(rootModelIndex());

        const int rowCount = count();
        const int visibleRows = qMin(qMax(rowCount, 1), 6);
        const int rowHeight = rowCount > 0
            ? qMax(38, m_popupView->sizeHintForRow(0) + 6)
            : 38;
        const int popupHeight = visibleRows * rowHeight + 20;
        const int popupWidth = qMax(width(), 240);

        m_popupFrame->resize(popupWidth, popupHeight);

        QPoint popupPoint = mapToGlobal(QPoint(0, height() + 6));
        if (QScreen *screen = this->screen()) {
            const QRect geometry = screen->availableGeometry();
            if (popupPoint.y() + popupHeight > geometry.bottom()) {
                popupPoint.setY(mapToGlobal(QPoint(0, -popupHeight - 6)).y());
            }
            if (popupPoint.x() + popupWidth > geometry.right()) {
                popupPoint.setX(geometry.right() - popupWidth);
            }
            if (popupPoint.x() < geometry.left()) {
                popupPoint.setX(geometry.left());
            }
        }

        m_popupFrame->move(popupPoint);
        m_popupFrame->show();
        m_popupFrame->raise();

        const QModelIndex current = model()->index(currentIndex(), modelColumn(), rootModelIndex());
        if (current.isValid()) {
            m_popupView->setCurrentIndex(current);
            m_popupView->scrollTo(current, QAbstractItemView::PositionAtCenter);
        }
    }

    void hidePopup() override
    {
        if (m_popupFrame) {
            m_popupFrame->hide();
        }
    }

private:
    void ensurePopup()
    {
        if (m_popupFrame) {
            return;
        }

        m_popupFrame = new QFrame(nullptr, Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
        m_popupFrame->setObjectName("styledComboPopup");
        m_popupFrame->setStyleSheet(
            "QFrame#styledComboPopup {"
            " background: #ffffff;"
            " border: 1px solid #dbe4f0;"
            " border-radius: 16px;"
            "}");

        auto *layout = new QHBoxLayout(m_popupFrame);
        layout->setContentsMargins(0, 0, 0, 0);

        m_popupView = new QListView(m_popupFrame);
        m_popupView->setFrameShape(QFrame::NoFrame);
        m_popupView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_popupView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_popupView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_popupView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        m_popupView->setStyleSheet(comboPopupViewStyle());

        layout->addWidget(m_popupView);

        connect(m_popupView, &QListView::clicked, m_popupFrame, [this](const QModelIndex &index) {
            if (index.isValid()) {
                setCurrentIndex(index.row());
            }
            hidePopup();
        });
    }

    QFrame *m_popupFrame = nullptr;
    QListView *m_popupView = nullptr;
};

inline QComboBox *createComboBox(QWidget *parent = nullptr)
{
    return new StyledComboBox(parent);
}

inline void applyComboBoxStyle(QComboBox *comboBox)
{
    if (!comboBox) {
        return;
    }

    comboBox->setStyleSheet(comboBoxStyle());
}

} // namespace ui_styles
