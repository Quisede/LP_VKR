#include "teachercreatecoursepage.h"

#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

TeacherCreateCoursePage::TeacherCreateCoursePage(QWidget *parent)
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

    auto *titleLabel = new QLabel("Создание курса", pageCard);
    titleLabel->setObjectName("sectionTitleLabel");

    auto *hintLabel = new QLabel(
        "Сформируй курс сразу в teacher-кабинете: сначала название и описание, потом мы откроем конструктор для уроков, материалов и тестов.",
        pageCard);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    auto *formCard = new QFrame(pageCard);
    formCard->setObjectName("profileInfoCard");
    auto *formLayout = new QFormLayout(formCard);
    formLayout->setHorizontalSpacing(18);
    formLayout->setVerticalSpacing(14);

    const QString inputStyle =
        "QLineEdit, QTextEdit {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " font-size: 14px;"
        "}";

    m_titleEdit = new QLineEdit(formCard);
    m_titleEdit->setPlaceholderText("Например: Modern C++");
    m_titleEdit->setStyleSheet(inputStyle);

    m_descriptionEdit = new QTextEdit(formCard);
    m_descriptionEdit->setPlaceholderText("Коротко опиши, чему научатся студенты и что входит в курс.");
    m_descriptionEdit->setMinimumHeight(160);
    m_descriptionEdit->setStyleSheet(inputStyle);

    m_createButton = new QPushButton("Создать курс", formCard);
    m_createButton->setObjectName("cardAccentButton");

    m_statusLabel = new QLabel("После создания курс сразу появится в разделе \"Мои курсы\".", pageCard);
    m_statusLabel->setObjectName("sectionHintLabel");
    m_statusLabel->setWordWrap(true);

    formLayout->addRow("Название курса", m_titleEdit);
    formLayout->addRow("Описание", m_descriptionEdit);
    formLayout->addRow("", m_createButton);

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);
    layout->addWidget(formCard);
    layout->addWidget(m_statusLabel);
    rootLayout->addWidget(pageCard);

    connect(m_titleEdit, &QLineEdit::textChanged, this, [this]() {
        updateButtonState();
    });
    connect(m_descriptionEdit, &QTextEdit::textChanged, this, [this]() {
        updateButtonState();
    });
    connect(m_createButton, &QPushButton::clicked, this, [this]() {
        emit createCourseRequested(
            m_titleEdit->text().trimmed(),
            m_descriptionEdit->toPlainText().trimmed());
    });

    updateButtonState();
}

void TeacherCreateCoursePage::clearForm()
{
    m_titleEdit->clear();
    m_descriptionEdit->clear();
    updateButtonState();
}

void TeacherCreateCoursePage::setBusy(bool busy)
{
    m_busy = busy;
    m_titleEdit->setEnabled(!busy);
    m_descriptionEdit->setEnabled(!busy);
    updateButtonState();
}

void TeacherCreateCoursePage::showMessage(const QString &message, bool error)
{
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

void TeacherCreateCoursePage::updateButtonState()
{
    const bool hasTitle = !m_titleEdit->text().trimmed().isEmpty();
    const bool hasDescription = !m_descriptionEdit->toPlainText().trimmed().isEmpty();
    m_createButton->setEnabled(!m_busy && hasTitle && hasDescription);
}
