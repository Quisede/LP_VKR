#include "teachercreatecoursepage.h"

#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "../models/coursemodel.h"

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

    m_titleLabel = new QLabel("Создание курса", pageCard);
    m_titleLabel->setObjectName("sectionTitleLabel");

    m_hintLabel = new QLabel(
        "Сформируй курс сразу в teacher-кабинете: сначала название и описание, потом мы откроем конструктор для уроков, материалов и тестов.",
        pageCard);
    m_hintLabel->setObjectName("sectionHintLabel");
    m_hintLabel->setWordWrap(true);

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

    m_submitButton = new QPushButton("Создать курс", formCard);
    m_submitButton->setObjectName("cardAccentButton");
    m_deleteButton = new QPushButton("Удалить курс", formCard);
    m_deleteButton->setObjectName("cardDangerButton");

    m_statusLabel = new QLabel("После создания курс сразу появится в разделе \"Мои курсы\".", pageCard);
    m_statusLabel->setObjectName("sectionHintLabel");
    m_statusLabel->setWordWrap(true);

    auto *buttonsRow = new QHBoxLayout();
    buttonsRow->setSpacing(12);
    buttonsRow->addWidget(m_submitButton);
    buttonsRow->addWidget(m_deleteButton);
    buttonsRow->addStretch();

    formLayout->addRow("Название курса", m_titleEdit);
    formLayout->addRow("Описание", m_descriptionEdit);

    layout->addWidget(m_titleLabel);
    layout->addWidget(m_hintLabel);
    layout->addWidget(formCard);
    layout->addLayout(buttonsRow);
    layout->addWidget(m_statusLabel);
    rootLayout->addWidget(pageCard);

    connect(m_titleEdit, &QLineEdit::textChanged, this, [this]() {
        updateButtonState();
    });
    connect(m_descriptionEdit, &QTextEdit::textChanged, this, [this]() {
        updateButtonState();
    });
    connect(m_submitButton, &QPushButton::clicked, this, [this]() {
        if (m_editMode) {
            emit updateCourseRequested(
                m_editingCourseId,
                m_titleEdit->text().trimmed(),
                m_descriptionEdit->toPlainText().trimmed());
        } else {
            emit createCourseRequested(
                m_titleEdit->text().trimmed(),
                m_descriptionEdit->toPlainText().trimmed());
        }
    });
    connect(m_deleteButton, &QPushButton::clicked, this, [this]() {
        if (m_editMode && m_editingCourseId >= 0) {
            emit deleteCourseRequested(m_editingCourseId);
        }
    });

    setCreateMode();
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

void TeacherCreateCoursePage::setCreateMode()
{
    m_editMode = false;
    m_editingCourseId = -1;
    m_titleLabel->setText("Создание курса");
    m_hintLabel->setText("Сформируй курс сразу в teacher-кабинете: сначала название и описание, потом мы откроем конструктор для уроков, материалов и тестов.");
    m_submitButton->setText("Создать курс");
    m_deleteButton->hide();
    m_statusLabel->setText("После создания курс сразу появится в разделе \"Мои курсы\".");
    clearForm();
}

void TeacherCreateCoursePage::setEditMode(const CourseData &course)
{
    m_editMode = true;
    m_editingCourseId = course.id;
    m_titleLabel->setText("Редактирование курса");
    m_hintLabel->setText("Обнови название и описание курса. При необходимости курс можно удалить вместе с его структурой.");
    m_submitButton->setText("Сохранить изменения");
    m_deleteButton->show();
    m_titleEdit->setText(course.title);
    m_descriptionEdit->setText(course.description);
    m_statusLabel->setText("После сохранения карточка курса и конструктор сразу обновятся.");
    updateButtonState();
}

bool TeacherCreateCoursePage::isEditMode() const
{
    return m_editMode;
}

int TeacherCreateCoursePage::editingCourseId() const
{
    return m_editingCourseId;
}

void TeacherCreateCoursePage::updateButtonState()
{
    const bool hasTitle = !m_titleEdit->text().trimmed().isEmpty();
    const bool hasDescription = !m_descriptionEdit->toPlainText().trimmed().isEmpty();
    m_submitButton->setEnabled(!m_busy && hasTitle && hasDescription);
    m_deleteButton->setEnabled(!m_busy && m_editMode && m_editingCourseId >= 0);
}
