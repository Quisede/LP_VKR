#include "teachercoursebuilderpage.h"
#include "../ui/uistyles.h"

#include <QComboBox>
#include <QCheckBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSpinBox>
#include <QTabBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QDialog>
#include <QDialogButtonBox>
#include <utility>

namespace {

constexpr qint64 kMaxEmbeddedMaterialBytes = 5 * 1024 * 1024;

void appendBuilderCard(QListWidget *list, const QString &title, const QString &subtitle)
{
    auto *item = new QListWidgetItem();
    item->setSizeHint(QSize(0, 92));

    auto *card = new QFrame(list);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(6);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");
    titleLabel->setWordWrap(true);

    auto *subtitleLabel = new QLabel(subtitle, card);
    subtitleLabel->setObjectName("courseCardDescriptionLabel");
    subtitleLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);

    list->addItem(item);
    list->setItemWidget(item, card);
}

QString materialTypeFromSuffix(const QString &suffix)
{
    const QString normalized = suffix.toLower();
    if (normalized == "pdf") {
        return "pdf";
    }
    if (normalized == "doc") {
        return "doc";
    }
    if (normalized == "docx") {
        return "docx";
    }
    if (normalized == "txt" || normalized == "md") {
        return "text";
    }
    return "file";
}

QString materialMimeFromType(const QString &type)
{
    if (type == "pdf") {
        return "application/pdf";
    }
    if (type == "doc") {
        return "application/msword";
    }
    if (type == "docx") {
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    }
    if (type == "text") {
        return "text/plain";
    }
    return "application/octet-stream";
}

QString normalizedBuilderText(QString text)
{
    text.replace("\\r\\n", "\n");
    text.replace("\\n", "\n");
    text.replace("\\t", "    ");
    return text.trimmed();
}

QString compactBuilderPreview(const QString &rawText, int limit = 180)
{
    QString text = normalizedBuilderText(rawText);
    text.replace('\n', " ");
    text = text.simplified();
    if (text.size() <= limit) {
        return text;
    }
    return text.left(limit).trimmed() + "...";
}

bool parseEmbeddedFileMaterial(const QString &content, QJsonObject *payload)
{
    const QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (!doc.isObject()) {
        return false;
    }

    const QJsonObject object = doc.object();
    if (object.value("kind").toString() != "embedded-file") {
        return false;
    }

    if (payload != nullptr) {
        *payload = object;
    }
    return true;
}

QString materialPreviewText(const MaterialData &material)
{
    QJsonObject filePayload;
    if (parseEmbeddedFileMaterial(material.content, &filePayload)) {
        const QString fileName = filePayload.value("fileName").toString(material.title);
        const QString mime = filePayload.value("mimeType").toString("application/octet-stream");
        const int size = filePayload.value("size").toInt(0);
        return QString("Тип: %1\nФайл: %2\nMIME: %3\nРазмер: %4 КБ\n\nФайл встроен в материал и будет доступен студенту после загрузки курса.")
            .arg(material.type)
            .arg(fileName)
            .arg(mime)
            .arg((size + 1023) / 1024);
    }

    return QString("Тип: %1\n\n%2")
        .arg(material.type.isEmpty() ? "text" : material.type)
        .arg(material.content.isEmpty() ? "Материал пока пуст." : normalizedBuilderText(material.content));
}

QFrame *createSectionCard(const QString &title, QWidget *parent, QVBoxLayout **contentLayout)
{
    auto *card = new QFrame(parent);
    card->setObjectName("moduleCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("moduleTitleLabel");

    layout->addWidget(titleLabel);
    *contentLayout = layout;
    return card;
}

QLabel *createFieldLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName("moduleTitleLabel");
    return label;
}

void applyLightDialogStyle(QDialog *dialog)
{
    dialog->setStyleSheet(
        "QDialog {"
        " background: #f8fbff;"
        " color: #0f172a;"
        "}"
        "QLabel { color: #334155; }"
        "QLabel#moduleTitleLabel {"
        " color: #334155;"
        " font-size: 13px;"
        " font-weight: 700;"
        "}"
        "QLabel#sectionHintLabel {"
        " color: #475569;"
        " font-size: 13px;"
        " font-weight: 600;"
        "}"
        "QLineEdit, QTextEdit, QDateTimeEdit, QSpinBox {"
        " background: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " selection-background-color: #bfdbfe;"
        "}"
        "QLineEdit:focus, QTextEdit:focus, QDateTimeEdit:focus, QSpinBox:focus {"
        " border-color: #2563eb;"
        "}"
        "QCheckBox {"
        " color: #0f172a;"
        " font-weight: 600;"
        " spacing: 8px;"
        "}"
        "QDialogButtonBox QPushButton, QPushButton#cardGhostButton {"
        " background: #eaf2ff;"
        " color: #2563eb;"
        " border: 1px solid #bfdbfe;"
        " border-radius: 14px;"
        " padding: 11px 18px;"
        " font-weight: 700;"
        " min-height: 20px;"
        "}"
        "QDialogButtonBox QPushButton:hover, QPushButton#cardGhostButton:hover {"
        " background: #dbeafe;"
        "}"
        "QDialogButtonBox QPushButton:default {"
        " background: #2563eb;"
        " color: #ffffff;"
        " border-color: #2563eb;"
        "}"
        "QDialogButtonBox QPushButton:disabled, QPushButton#cardGhostButton:disabled {"
        " background: #e2e8f0;"
        " color: #94a3b8;"
        " border-color: #e2e8f0;"
        "}");
}

void applyTransparentListStyle(QListWidget *list)
{
    list->setFrameShape(QFrame::NoFrame);
    list->setAttribute(Qt::WA_StyledBackground, true);
    list->viewport()->setAttribute(Qt::WA_StyledBackground, true);
    list->setStyleSheet(
        "QListWidget { background: transparent; border: none; outline: none; }"
        "QListWidget::item { background: transparent; border: none; margin: 0; padding: 0; }"
        "QListWidget::item:hover, QListWidget::item:selected { background: transparent; }");
    list->viewport()->setStyleSheet("background: transparent;");
}

QString statusWord(bool ready)
{
    return ready ? "готово" : "ожидает";
}

bool editLessonDialog(
    QWidget *parent,
    const QString &windowTitle,
    const QString &initialTitle,
    const QString &initialContent,
    QString *titleOut,
    QString *contentOut)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(windowTitle);
    dialog.resize(720, 560);
    applyLightDialogStyle(&dialog);

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(22, 22, 22, 18);
    layout->setSpacing(14);

    auto *hintLabel = new QLabel("Заполните название и основной текст урока. Так урок будет читаться студентом в отдельном reader-окне.", &dialog);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    auto *titleEdit = new QLineEdit(&dialog);
    titleEdit->setPlaceholderText("Например: Урок 1. Переменные и типы данных");
    titleEdit->setText(initialTitle);

    auto *contentEdit = new QTextEdit(&dialog);
    contentEdit->setPlaceholderText("Основной учебный текст урока");
    contentEdit->setPlainText(initialContent);
    contentEdit->setMinimumHeight(320);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Save)->setText("Сохранить");
    buttons->button(QDialogButtonBox::Cancel)->setText("Отмена");

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
        if (titleEdit->text().trimmed().isEmpty() || contentEdit->toPlainText().trimmed().isEmpty()) {
            return;
        }
        dialog.accept();
    });
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(hintLabel);
    layout->addWidget(createFieldLabel("Название урока", &dialog));
    layout->addWidget(titleEdit);
    layout->addWidget(createFieldLabel("Текст урока", &dialog));
    layout->addWidget(contentEdit, 1);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    *titleOut = titleEdit->text().trimmed();
    *contentOut = contentEdit->toPlainText().trimmed();
    return true;
}

bool editMaterialDialog(
    QWidget *parent,
    const QString &windowTitle,
    const QVector<LessonData> &lessons,
    int initialLessonId,
    const QString &initialTitle,
    const QString &initialType,
    const QString &initialContent,
    int *lessonIdOut,
    QString *titleOut,
    QString *typeOut,
    QString *contentOut)
{
    if (lessons.isEmpty()) {
        return false;
    }

    QDialog dialog(parent);
    dialog.setWindowTitle(windowTitle);
    dialog.resize(760, 640);
    applyLightDialogStyle(&dialog);

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(22, 22, 22, 18);
    layout->setSpacing(14);

    auto *hintLabel = new QLabel("Материал привязывается к конкретному уроку. Можно добавить текст, ссылку, видео или прикрепить PDF/DOC/DOCX/TXT файл.", &dialog);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    auto *lessonCombo = ui_styles::createComboBox(&dialog);
    ui_styles::applyComboBoxStyle(lessonCombo);
    for (const LessonData &lesson : lessons) {
        lessonCombo->addItem(lesson.title, lesson.id);
    }
    for (int i = 0; i < lessonCombo->count(); ++i) {
        if (lessonCombo->itemData(i).toInt() == initialLessonId) {
            lessonCombo->setCurrentIndex(i);
            break;
        }
    }

    auto *titleEdit = new QLineEdit(&dialog);
    titleEdit->setPlaceholderText("Название материала");
    titleEdit->setText(initialTitle);

    auto *typeCombo = ui_styles::createComboBox(&dialog);
    typeCombo->addItems({"text", "video", "link", "pdf", "doc", "docx", "file"});
    ui_styles::applyComboBoxStyle(typeCombo);
    const int typeIndex = typeCombo->findText(initialType);
    typeCombo->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);

    auto *contentEdit = new QTextEdit(&dialog);
    contentEdit->setPlaceholderText("Текст материала или ссылка");
    contentEdit->setPlainText(initialContent);
    contentEdit->setMinimumHeight(240);

    QString embeddedPayload;
    QJsonObject payload;
    if (parseEmbeddedFileMaterial(initialContent, &payload)) {
        embeddedPayload = initialContent;
        contentEdit->setPlainText(QString("Встроенный файл: %1\nРазмер: %2 КБ")
            .arg(payload.value("fileName").toString(initialTitle))
            .arg((payload.value("size").toInt(0) + 1023) / 1024));
    }

    auto *pickFileButton = new QPushButton("Выбрать файл с компьютера", &dialog);
    pickFileButton->setObjectName("cardGhostButton");

    QObject::connect(contentEdit, &QTextEdit::textChanged, &dialog, [&]() {
        if (!contentEdit->toPlainText().startsWith("Встроенный файл:")) {
            embeddedPayload.clear();
        }
    });

    QObject::connect(pickFileButton, &QPushButton::clicked, &dialog, [&]() {
        const QString path = QFileDialog::getOpenFileName(
            &dialog,
            "Выбрать материал",
            QString(),
            "Учебные материалы (*.pdf *.doc *.docx *.txt *.md);;PDF (*.pdf);;Word (*.doc *.docx);;Текст (*.txt *.md);;Все файлы (*)");
        if (path.isEmpty()) {
            return;
        }

        QFileInfo fileInfo(path);
        if (!fileInfo.exists() || fileInfo.size() > kMaxEmbeddedMaterialBytes) {
            return;
        }

        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)) {
            return;
        }

        const QByteArray bytes = file.readAll();
        const QString type = materialTypeFromSuffix(fileInfo.suffix());
        const QJsonObject filePayload{
            {"kind", "embedded-file"},
            {"fileName", fileInfo.fileName()},
            {"mimeType", materialMimeFromType(type)},
            {"size", static_cast<int>(bytes.size())},
            {"data", QString::fromLatin1(bytes.toBase64())}
        };

        const int newTypeIndex = typeCombo->findText(type);
        typeCombo->setCurrentIndex(newTypeIndex >= 0 ? newTypeIndex : typeCombo->findText("file"));
        if (titleEdit->text().trimmed().isEmpty()) {
            titleEdit->setText(fileInfo.completeBaseName());
        }
        embeddedPayload = QString::fromUtf8(QJsonDocument(filePayload).toJson(QJsonDocument::Compact));
        contentEdit->setPlainText(QString("Встроенный файл: %1\nРазмер: %2 КБ")
            .arg(fileInfo.fileName())
            .arg((bytes.size() + 1023) / 1024));
    });

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Save)->setText("Сохранить");
    buttons->button(QDialogButtonBox::Cancel)->setText("Отмена");
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
        const QString submittedContent = embeddedPayload.isEmpty()
            ? contentEdit->toPlainText().trimmed()
            : embeddedPayload;
        if (titleEdit->text().trimmed().isEmpty() || submittedContent.isEmpty()) {
            return;
        }
        dialog.accept();
    });
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(hintLabel);
    layout->addWidget(createFieldLabel("Урок", &dialog));
    layout->addWidget(lessonCombo);
    layout->addWidget(createFieldLabel("Название", &dialog));
    layout->addWidget(titleEdit);
    layout->addWidget(createFieldLabel("Тип", &dialog));
    layout->addWidget(typeCombo);
    layout->addWidget(createFieldLabel("Содержимое", &dialog));
    layout->addWidget(contentEdit, 1);
    layout->addWidget(pickFileButton, 0, Qt::AlignLeft);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    *lessonIdOut = lessonCombo->currentData().toInt();
    *titleOut = titleEdit->text().trimmed();
    *typeOut = typeCombo->currentText();
    *contentOut = embeddedPayload.isEmpty() ? contentEdit->toPlainText().trimmed() : embeddedPayload;
    return true;
}

bool editTestDialog(
    QWidget *parent,
    const QString &windowTitle,
    const QString &initialTitle,
    const QString &initialStatus,
    const QString &initialDeadlineAt,
    int initialMaxAttempts,
    int initialTimeLimitMinutes,
    QString *titleOut,
    QString *statusOut,
    QString *deadlineAtOut,
    int *maxAttemptsOut,
    int *timeLimitMinutesOut)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(windowTitle);
    dialog.resize(640, 560);
    applyLightDialogStyle(&dialog);

    auto *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(22, 22, 22, 18);
    layout->setSpacing(14);

    auto *hintLabel = new QLabel("Укажите понятное название теста. Вопросы и варианты ответа редактируются в отдельном пошаговом редакторе.", &dialog);
    hintLabel->setObjectName("sectionHintLabel");
    hintLabel->setWordWrap(true);

    auto *titleEdit = new QLineEdit(&dialog);
    titleEdit->setPlaceholderText("Например: Итоговая проверка по теме");
    titleEdit->setText(initialTitle);
    titleEdit->setMinimumHeight(46);

    auto *statusCombo = ui_styles::createComboBox(&dialog);
    statusCombo->addItems({"active", "closed"});
    ui_styles::applyComboBoxStyle(statusCombo);
    const int statusIndex = statusCombo->findText(initialStatus.isEmpty() ? "active" : initialStatus);
    statusCombo->setCurrentIndex(statusIndex >= 0 ? statusIndex : 0);

    auto *deadlineEnabledCheck = new QCheckBox("Установить дедлайн", &dialog);
    auto *deadlineEdit = new QDateTimeEdit(&dialog);
    deadlineEdit->setCalendarPopup(true);
    deadlineEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    deadlineEdit->setMinimumDateTime(QDateTime::currentDateTime().addSecs(-86400));
    deadlineEdit->setDateTime(QDateTime::currentDateTime().addDays(7));

    if (!initialDeadlineAt.trimmed().isEmpty()) {
        QDateTime deadline = QDateTime::fromString(initialDeadlineAt, Qt::ISODate);
        if (!deadline.isValid()) {
            deadline = QDateTime::fromString(initialDeadlineAt, "yyyy-MM-dd HH:mm:ss");
        }
        if (deadline.isValid()) {
            deadlineEnabledCheck->setChecked(true);
            deadlineEdit->setDateTime(deadline);
        }
    }
    deadlineEdit->setEnabled(deadlineEnabledCheck->isChecked());
    QObject::connect(deadlineEnabledCheck, &QCheckBox::toggled, deadlineEdit, &QDateTimeEdit::setEnabled);

    auto *maxAttemptsSpin = new QSpinBox(&dialog);
    maxAttemptsSpin->setRange(0, 20);
    maxAttemptsSpin->setValue(initialMaxAttempts);
    maxAttemptsSpin->setSpecialValueText("Без ограничения");
    maxAttemptsSpin->setMinimumHeight(46);

    auto *timeLimitSpin = new QSpinBox(&dialog);
    timeLimitSpin->setRange(1, 300);
    timeLimitSpin->setValue(qBound(1, initialTimeLimitMinutes, 300));
    timeLimitSpin->setSuffix(" мин.");
    timeLimitSpin->setMinimumHeight(46);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Save)->setText("Сохранить");
    buttons->button(QDialogButtonBox::Cancel)->setText("Отмена");

    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, [&]() {
        if (titleEdit->text().trimmed().isEmpty()) {
            titleEdit->setFocus();
            return;
        }
        dialog.accept();
    });
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(hintLabel);
    layout->addWidget(createFieldLabel("Название теста", &dialog));
    layout->addWidget(titleEdit);
    layout->addWidget(createFieldLabel("Статус", &dialog));
    layout->addWidget(statusCombo);
    layout->addWidget(deadlineEnabledCheck);
    layout->addWidget(deadlineEdit);
    layout->addWidget(createFieldLabel("Количество попыток", &dialog));
    layout->addWidget(maxAttemptsSpin);
    layout->addWidget(createFieldLabel("Время на прохождение", &dialog));
    layout->addWidget(timeLimitSpin);
    layout->addStretch();
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    *titleOut = titleEdit->text().trimmed();
    *statusOut = statusCombo->currentText();
    *deadlineAtOut = deadlineEnabledCheck->isChecked()
        ? deadlineEdit->dateTime().toString(Qt::ISODate)
        : QString();
    *maxAttemptsOut = maxAttemptsSpin->value();
    *timeLimitMinutesOut = timeLimitSpin->value();
    return true;
}

}

TeacherCourseBuilderPage::TeacherCourseBuilderPage(QWidget *parent)
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

    const QString inputStyle =
        "QLineEdit, QTextEdit, QComboBox {"
        " background-color: #ffffff;"
        " color: #0f172a;"
        " border: 1px solid #dbe4f0;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        " font-size: 14px;"
        "}"
        "QComboBox::drop-down {"
        " width: 34px;"
        " border: none;"
        " background: transparent;"
        "}";

    m_courseTitleLabel = new QLabel("Конструктор курса", pageCard);
    m_courseTitleLabel->setObjectName("sectionTitleLabel");

    m_courseDescriptionLabel = new QLabel(
        "Выберите курс во вкладке \"Мои курсы\", чтобы начать оформление структуры курса.",
        pageCard);
    m_courseDescriptionLabel->setObjectName("sectionHintLabel");
    m_courseDescriptionLabel->setWordWrap(true);

    m_messageLabel = new QLabel("Здесь можно добавлять уроки, материалы и тесты для выбранного курса.", pageCard);
    m_messageLabel->setObjectName("sectionHintLabel");
    m_messageLabel->setWordWrap(true);

    auto *tabs = new QTabWidget(pageCard);
    tabs->setDocumentMode(true);
    tabs->setElideMode(Qt::ElideNone);
    tabs->tabBar()->setExpanding(false);
    tabs->tabBar()->setDrawBase(false);
    tabs->setStyleSheet(
        "QTabWidget::pane {"
        " border: 1px solid #dbe4f0;"
        " border-radius: 18px;"
        " margin-top: 14px;"
        " background: #f8fbff;"
        " padding: 18px 18px 12px 18px;"
        "}"
        "QTabWidget::tab-bar { alignment: left; left: 10px; }"
        "QTabBar::tab {"
        " background: #e2e8f0;"
        " color: #334155;"
        " border: none;"
        " border-top-left-radius: 12px;"
        " border-top-right-radius: 12px;"
        " padding: 11px 20px;"
        " min-width: 112px;"
        " font-weight: 600;"
        " margin-right: 8px;"
        " margin-bottom: 0px;"
        "}"
        "QTabBar::tab:selected {"
        " background: #2563eb;"
        " color: #ffffff;"
        "}"
        "QTabBar::tab:hover:!selected {"
        " background: #cbd5e1;"
        "}");

    auto *overviewTab = new QWidget(tabs);
    auto *overviewLayout = new QVBoxLayout(overviewTab);
    overviewLayout->setContentsMargins(0, 0, 0, 0);
    overviewLayout->setSpacing(14);

    auto *summaryLayout = new QHBoxLayout();
    summaryLayout->setSpacing(14);

    auto createSummaryCard = [overviewTab](const QString &title, QLabel **valueLabel) {
        auto *card = new QFrame(overviewTab);
        card->setObjectName("moduleCard");
        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 18, 18, 18);
        cardLayout->setSpacing(8);

        auto *titleLabel = new QLabel(title, card);
        titleLabel->setObjectName("moduleTitleLabel");

        *valueLabel = new QLabel("0", card);
        (*valueLabel)->setObjectName("courseDetailTitleLabel");

        cardLayout->addWidget(titleLabel);
        cardLayout->addWidget(*valueLabel);
        cardLayout->addStretch();
        return card;
    };

    summaryLayout->addWidget(createSummaryCard("Уроки", &m_lessonsSummaryLabel));
    summaryLayout->addWidget(createSummaryCard("Материалы", &m_materialsSummaryLabel));
    summaryLayout->addWidget(createSummaryCard("Тесты", &m_testsSummaryLabel));

    auto *overviewNote = new QLabel(
        "Структура курса собирается по шагам: сначала уроки, затем привязка материалов и после этого тесты.",
        overviewTab);
    overviewNote->setObjectName("sectionHintLabel");
    overviewNote->setWordWrap(true);

    m_stageChecklistLabel = new QLabel(overviewTab);
    m_stageChecklistLabel->setObjectName("sectionHintLabel");
    m_stageChecklistLabel->setWordWrap(true);
    m_stageChecklistLabel->setStyleSheet(
        "QLabel {"
        " background: #eff6ff;"
        " color: #1e3a8a;"
        " border: 1px solid #bfdbfe;"
        " border-radius: 16px;"
        " padding: 14px 16px;"
        " line-height: 1.4;"
        "}");

    overviewLayout->addLayout(summaryLayout);
    overviewLayout->addWidget(overviewNote);
    overviewLayout->addWidget(m_stageChecklistLabel);
    overviewLayout->addStretch();

    auto *lessonsTab = new QWidget(tabs);
    auto *lessonsLayout = new QHBoxLayout(lessonsTab);
    lessonsLayout->setContentsMargins(0, 0, 0, 0);
    lessonsLayout->setSpacing(14);

    QVBoxLayout *lessonsCardLayout = nullptr;
    auto *lessonsCard = createSectionCard("Уроки курса", lessonsTab, &lessonsCardLayout);
    m_lessonsList = new QListWidget(lessonsCard);
    m_lessonsList->setSpacing(10);
    m_lessonsList->setMinimumWidth(360);
    applyTransparentListStyle(m_lessonsList);
    lessonsCardLayout->addWidget(m_lessonsList);

    QVBoxLayout *lessonFormLayout = nullptr;
    auto *lessonFormCard = createSectionCard("Действия с уроками", lessonsTab, &lessonFormLayout);
    auto *lessonFormHint = new QLabel(
        "Выберите урок слева, чтобы редактировать или удалить его. Создание и редактирование открываются в отдельном окне.",
        lessonFormCard);
    lessonFormHint->setObjectName("sectionHintLabel");
    lessonFormHint->setWordWrap(true);

    m_lessonTitleEdit = new QLineEdit(lessonFormCard);
    m_lessonTitleEdit->setPlaceholderText("Название урока");
    m_lessonTitleEdit->setStyleSheet(inputStyle);
    m_lessonTitleEdit->setMinimumHeight(48);
    m_lessonTitleEdit->hide();

    m_lessonContentEdit = new QTextEdit(lessonFormCard);
    m_lessonContentEdit->setPlaceholderText("Текст, краткое описание или основное содержание урока");
    m_lessonContentEdit->setMinimumHeight(180);
    m_lessonContentEdit->setStyleSheet(inputStyle);
    m_lessonContentEdit->hide();

    m_addLessonButton = new QPushButton("Добавить новый урок", lessonFormCard);
    m_addLessonButton->setObjectName("cardAccentButton");
    m_loadLessonButton = new QPushButton("Редактировать выбранный", lessonFormCard);
    m_loadLessonButton->setObjectName("cardGhostButton");
    m_updateLessonButton = new QPushButton("Сохранить изменения урока", lessonFormCard);
    m_updateLessonButton->setObjectName("cardAccentButton");
    m_updateLessonButton->hide();
    m_deleteLessonButton = new QPushButton("Удалить урок", lessonFormCard);
    m_deleteLessonButton->setObjectName("cardDangerButton");

    lessonFormLayout->addWidget(lessonFormHint);
    lessonFormLayout->addWidget(m_addLessonButton);
    lessonFormLayout->addWidget(m_loadLessonButton);
    lessonFormLayout->addWidget(m_deleteLessonButton);
    lessonFormLayout->addStretch();

    lessonsLayout->addWidget(lessonsCard, 3);
    lessonsLayout->addWidget(lessonFormCard, 2);

    auto *materialsTab = new QWidget(tabs);
    auto *materialsLayout = new QHBoxLayout(materialsTab);
    materialsLayout->setContentsMargins(0, 0, 0, 0);
    materialsLayout->setSpacing(14);

    QVBoxLayout *materialsCardLayout = nullptr;
    auto *materialsCard = createSectionCard("Материалы урока", materialsTab, &materialsCardLayout);
    m_materialLessonCombo = ui_styles::createComboBox(materialsCard);
    ui_styles::applyComboBoxStyle(m_materialLessonCombo);
    m_materialsList = new QListWidget(materialsCard);
    m_materialsList->setSpacing(10);
    m_materialsList->setMinimumWidth(360);
    applyTransparentListStyle(m_materialsList);
    materialsCardLayout->addWidget(m_materialLessonCombo);
    materialsCardLayout->addWidget(m_materialsList);

    QVBoxLayout *materialFormLayout = nullptr;
    auto *materialFormCard = createSectionCard("Действия с материалами", materialsTab, &materialFormLayout);
    auto *materialFormHint = new QLabel(
        "Выберите урок и материал слева. Создание и редактирование открываются в отдельном окне; файлы можно прикреплять прямо там.",
        materialFormCard);
    materialFormHint->setObjectName("sectionHintLabel");
    materialFormHint->setWordWrap(true);
    m_materialsGuardLabel = new QLabel(materialFormCard);
    m_materialsGuardLabel->setObjectName("sectionHintLabel");
    m_materialsGuardLabel->setWordWrap(true);
    m_materialsGuardLabel->setStyleSheet(
        "QLabel {"
        " background: #fff7ed;"
        " color: #9a3412;"
        " border: 1px solid #fed7aa;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        "}");
    m_materialTitleEdit = new QLineEdit(materialFormCard);
    m_materialTitleEdit->setPlaceholderText("Например: Теория по указателям");
    m_materialTitleEdit->setStyleSheet(inputStyle);
    m_materialTitleEdit->setMinimumHeight(48);
    m_materialTitleEdit->hide();

    m_materialTypeCombo = ui_styles::createComboBox(materialFormCard);
    m_materialTypeCombo->addItems({"text", "video", "link", "pdf", "doc", "docx", "file"});
    ui_styles::applyComboBoxStyle(m_materialTypeCombo);
    m_materialTypeCombo->hide();

    m_materialContentEdit = new QTextEdit(materialFormCard);
    m_materialContentEdit->setPlaceholderText("Текст материала, ссылка на видео или полезный ресурс");
    m_materialContentEdit->setMinimumHeight(180);
    m_materialContentEdit->setStyleSheet(inputStyle);
    m_materialContentEdit->hide();

    m_materialPreviewEdit = new QTextEdit(materialFormCard);
    m_materialPreviewEdit->setReadOnly(true);
    m_materialPreviewEdit->setMinimumHeight(120);
    m_materialPreviewEdit->setPlaceholderText("Здесь появится предпросмотр выбранного материала.");
    m_materialPreviewEdit->setStyleSheet(inputStyle + "QTextEdit { background-color: #f8fbff; }");
    m_materialPreviewEdit->hide();

    m_addMaterialButton = new QPushButton("Добавить новый материал", materialFormCard);
    m_addMaterialButton->setObjectName("cardAccentButton");
    m_pickMaterialFileButton = new QPushButton("Выбрать файл", materialFormCard);
    m_pickMaterialFileButton->setObjectName("cardGhostButton");
    m_pickMaterialFileButton->hide();
    m_loadMaterialButton = new QPushButton("Редактировать выбранный", materialFormCard);
    m_loadMaterialButton->setObjectName("cardGhostButton");
    m_previewMaterialButton = new QPushButton("Предпросмотр", materialFormCard);
    m_previewMaterialButton->setObjectName("cardGhostButton");
    m_openMaterialButton = new QPushButton("Открыть", materialFormCard);
    m_openMaterialButton->setObjectName("cardGhostButton");
    m_downloadMaterialButton = new QPushButton("Скачать", materialFormCard);
    m_downloadMaterialButton->setObjectName("cardGhostButton");
    m_updateMaterialButton = new QPushButton("Сохранить изменения материала", materialFormCard);
    m_updateMaterialButton->setObjectName("cardAccentButton");
    m_updateMaterialButton->hide();
    m_deleteMaterialButton = new QPushButton("Удалить материал", materialFormCard);
    m_deleteMaterialButton->setObjectName("cardDangerButton");

    materialFormLayout->addWidget(materialFormHint);
    materialFormLayout->addWidget(m_materialsGuardLabel);
    materialFormLayout->addWidget(m_addMaterialButton);
    materialFormLayout->addWidget(m_loadMaterialButton);
    materialFormLayout->addWidget(m_previewMaterialButton);
    materialFormLayout->addWidget(m_openMaterialButton);
    materialFormLayout->addWidget(m_downloadMaterialButton);
    materialFormLayout->addWidget(m_deleteMaterialButton);
    materialFormLayout->addStretch();

    materialsLayout->addWidget(materialsCard, 3);
    materialsLayout->addWidget(materialFormCard, 2);

    auto *testsTab = new QWidget(tabs);
    auto *testsLayout = new QHBoxLayout(testsTab);
    testsLayout->setContentsMargins(0, 0, 0, 0);
    testsLayout->setSpacing(14);

    QVBoxLayout *testsColumnLayout = new QVBoxLayout();
    testsColumnLayout->setSpacing(14);

    QVBoxLayout *testsCardLayout = nullptr;
    auto *testsCard = createSectionCard("Тесты курса", testsTab, &testsCardLayout);
    m_testsList = new QListWidget(testsCard);
    m_testsList->setSpacing(10);
    m_testsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_testsList->setMinimumWidth(340);
    applyTransparentListStyle(m_testsList);
    testsCardLayout->addWidget(m_testsList);
    testsColumnLayout->addWidget(testsCard);

    QVBoxLayout *questionsCardLayout = nullptr;
    auto *questionsCard = createSectionCard("Вопросы выбранного теста", testsTab, &questionsCardLayout);
    m_questionsList = new QListWidget(questionsCard);
    m_questionsList->setSpacing(10);
    m_questionsList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_questionsList->setMinimumWidth(340);
    applyTransparentListStyle(m_questionsList);
    questionsCardLayout->addWidget(m_questionsList);
    testsColumnLayout->addWidget(questionsCard);

    auto *rightColumnLayout = new QVBoxLayout();
    rightColumnLayout->setSpacing(14);

    QVBoxLayout *testFormLayout = nullptr;
    auto *testFormCard = createSectionCard("Действия с тестами", testsTab, &testFormLayout);
    auto *testFormHint = new QLabel(
        "Выберите тест слева, чтобы редактировать или удалить его. Вопросы удобнее вести в отдельном пошаговом редакторе.",
        testFormCard);
    testFormHint->setObjectName("sectionHintLabel");
    testFormHint->setWordWrap(true);
    auto *openEditorButton = new QPushButton("Перейти в пошаговый редактор тестов", testFormCard);
    openEditorButton->setObjectName("cardGhostButton");

    m_testTitleEdit = new QLineEdit(testFormCard);
    m_testTitleEdit->setPlaceholderText("Например: Финальный тест по теме");
    m_testTitleEdit->setStyleSheet(inputStyle);
    m_testTitleEdit->setMinimumHeight(48);
    m_testTitleEdit->hide();

    m_addTestButton = new QPushButton("Добавить новый тест", testFormCard);
    m_addTestButton->setObjectName("cardAccentButton");
    m_loadTestButton = new QPushButton("Редактировать выбранный", testFormCard);
    m_loadTestButton->setObjectName("cardGhostButton");
    m_updateTestButton = new QPushButton("Сохранить изменения теста", testFormCard);
    m_updateTestButton->setObjectName("cardAccentButton");
    m_updateTestButton->hide();
    m_deleteTestButton = new QPushButton("Удалить тест", testFormCard);
    m_deleteTestButton->setObjectName("cardDangerButton");

    testFormLayout->addWidget(testFormHint);
    testFormLayout->addWidget(m_addTestButton);
    testFormLayout->addWidget(m_loadTestButton);
    testFormLayout->addWidget(openEditorButton);
    testFormLayout->addWidget(m_deleteTestButton);
    testFormLayout->addStretch();
    rightColumnLayout->addWidget(testFormCard);

    QVBoxLayout *questionSummaryLayout = nullptr;
    auto *questionSummaryCard = createSectionCard("2. Вопросы и сценарий теста", testsTab, &questionSummaryLayout);
    auto *questionDivider = new QLabel(
        "Внутри конструктора оставляем только быстрый обзор. Полное редактирование вопросов удобнее делать в отдельном пошаговом редакторе.",
        questionSummaryCard);
    questionDivider->setObjectName("sectionHintLabel");
    questionDivider->setWordWrap(true);
    m_questionsGuardLabel = new QLabel(questionSummaryCard);
    m_questionsGuardLabel->setObjectName("sectionHintLabel");
    m_questionsGuardLabel->setWordWrap(true);
    m_questionsGuardLabel->setStyleSheet(
        "QLabel {"
        " background: #fff7ed;"
        " color: #9a3412;"
        " border: 1px solid #fed7aa;"
        " border-radius: 14px;"
        " padding: 10px 12px;"
        "}");

    m_questionTestCombo = ui_styles::createComboBox(questionSummaryCard);
    ui_styles::applyComboBoxStyle(m_questionTestCombo);
    m_questionTestCombo->setMinimumHeight(46);

    m_questionEditorSummaryLabel = new QLabel(questionSummaryCard);
    m_questionEditorSummaryLabel->setObjectName("sectionHintLabel");
    m_questionEditorSummaryLabel->setWordWrap(true);

    auto *openQuestionEditorButton = new QPushButton("Открыть отдельный редактор вопросов", questionSummaryCard);
    openQuestionEditorButton->setObjectName("cardAccentButton");

    questionSummaryLayout->addWidget(questionDivider);
    questionSummaryLayout->addWidget(m_questionsGuardLabel);
    questionSummaryLayout->addWidget(createFieldLabel("Тест для предпросмотра вопросов", questionSummaryCard));
    questionSummaryLayout->addWidget(m_questionTestCombo);
    questionSummaryLayout->addWidget(m_questionEditorSummaryLabel);
    questionSummaryLayout->addWidget(openQuestionEditorButton, 0, Qt::AlignLeft);
    questionSummaryLayout->addStretch();

    auto *questionFormCard = new QFrame(testsTab);
    questionFormCard->hide();
    auto *questionFormLayout = new QVBoxLayout(questionFormCard);
    questionFormLayout->setContentsMargins(0, 0, 0, 0);
    questionFormLayout->setSpacing(10);

    m_questionTextEdit = new QTextEdit(questionFormCard);
    m_questionTextEdit->setPlaceholderText("Текст вопроса");
    m_questionTextEdit->setMinimumHeight(110);
    m_questionTextEdit->setStyleSheet(inputStyle);

    m_optionOneEdit = new QLineEdit(questionFormCard);
    m_optionTwoEdit = new QLineEdit(questionFormCard);
    m_optionThreeEdit = new QLineEdit(questionFormCard);
    m_optionFourEdit = new QLineEdit(questionFormCard);
    for (QLineEdit *edit : {m_optionOneEdit, m_optionTwoEdit, m_optionThreeEdit, m_optionFourEdit}) {
        edit->setPlaceholderText("Вариант ответа");
        edit->setStyleSheet(inputStyle);
        edit->setMinimumHeight(44);
    }

    m_correctOptionCombo = ui_styles::createComboBox(questionFormCard);
    m_correctOptionCombo->addItems({"Вариант 1", "Вариант 2", "Вариант 3", "Вариант 4"});
    ui_styles::applyComboBoxStyle(m_correctOptionCombo);
    m_correctOptionCombo->setMinimumHeight(46);

    m_optionOrderCombo = ui_styles::createComboBox(questionFormCard);
    m_optionOrderCombo->addItems({"Вариант 1", "Вариант 2", "Вариант 3", "Вариант 4"});
    ui_styles::applyComboBoxStyle(m_optionOrderCombo);
    m_optionOrderCombo->setMinimumHeight(46);

    m_moveOptionUpButton = new QPushButton("Поднять вариант выше", questionFormCard);
    m_moveOptionUpButton->setObjectName("cardGhostButton");
    m_moveOptionDownButton = new QPushButton("Опустить вариант ниже", questionFormCard);
    m_moveOptionDownButton->setObjectName("cardGhostButton");

    m_addQuestionButton = new QPushButton("Сохранить вопрос", questionFormCard);
    m_addQuestionButton->setObjectName("cardAccentButton");
    m_loadQuestionButton = new QPushButton("Редактировать выбранный вопрос", questionFormCard);
    m_loadQuestionButton->setObjectName("cardGhostButton");
    m_updateQuestionButton = new QPushButton("Сохранить изменения вопроса", questionFormCard);
    m_updateQuestionButton->setObjectName("cardAccentButton");
    m_deleteQuestionButton = new QPushButton("Удалить вопрос", questionFormCard);
    m_deleteQuestionButton->setObjectName("cardDangerButton");

    auto *optionsGrid = new QGridLayout();
    optionsGrid->setHorizontalSpacing(12);
    optionsGrid->setVerticalSpacing(10);
    optionsGrid->addWidget(createFieldLabel("Вариант 1", questionFormCard), 0, 0);
    optionsGrid->addWidget(createFieldLabel("Вариант 2", questionFormCard), 0, 1);
    optionsGrid->addWidget(m_optionOneEdit, 1, 0);
    optionsGrid->addWidget(m_optionTwoEdit, 1, 1);
    optionsGrid->addWidget(createFieldLabel("Вариант 3", questionFormCard), 2, 0);
    optionsGrid->addWidget(createFieldLabel("Вариант 4", questionFormCard), 2, 1);
    optionsGrid->addWidget(m_optionThreeEdit, 3, 0);
    optionsGrid->addWidget(m_optionFourEdit, 3, 1);

    auto *answerSettingsColumn = new QVBoxLayout();
    answerSettingsColumn->setSpacing(10);
    answerSettingsColumn->addWidget(createFieldLabel("Правильный ответ", questionFormCard));
    answerSettingsColumn->addWidget(m_correctOptionCombo);
    answerSettingsColumn->addWidget(createFieldLabel("Какой вариант переставить", questionFormCard));
    answerSettingsColumn->addWidget(m_optionOrderCombo);
    auto *optionOrderRow = new QHBoxLayout();
    optionOrderRow->setSpacing(10);
    optionOrderRow->addWidget(m_moveOptionUpButton);
    optionOrderRow->addWidget(m_moveOptionDownButton);
    optionOrderRow->addStretch();
    auto *questionPrimaryRow = new QHBoxLayout();
    questionPrimaryRow->setSpacing(10);
    questionPrimaryRow->addWidget(m_addQuestionButton);
    questionPrimaryRow->addWidget(m_loadQuestionButton);
    questionPrimaryRow->addStretch();

    auto *questionDangerRow = new QHBoxLayout();
    questionDangerRow->setSpacing(10);
    questionDangerRow->addWidget(m_updateQuestionButton);
    questionDangerRow->addWidget(m_deleteQuestionButton);
    questionDangerRow->addStretch();

    questionFormLayout->addWidget(questionDivider);
    questionFormLayout->addWidget(m_questionsGuardLabel);
    questionFormLayout->addWidget(createFieldLabel("Тест, к которому относится вопрос", questionFormCard));
    questionFormLayout->addWidget(m_questionTestCombo);
    questionFormLayout->addWidget(createFieldLabel("Формулировка вопроса", questionFormCard));
    questionFormLayout->addWidget(m_questionTextEdit);
    questionFormLayout->addWidget(createFieldLabel("Варианты ответа", questionFormCard));
    questionFormLayout->addLayout(optionsGrid);
    questionFormLayout->addLayout(answerSettingsColumn);
    questionFormLayout->addLayout(optionOrderRow);
    questionFormLayout->addLayout(questionPrimaryRow);
    questionFormLayout->addLayout(questionDangerRow);
    questionFormLayout->addStretch();

    testsLayout->addLayout(testsColumnLayout, 3);
    rightColumnLayout->addWidget(questionSummaryCard);
    testsLayout->addLayout(rightColumnLayout, 2);

    tabs->addTab(overviewTab, "Обзор");
    tabs->addTab(lessonsTab, "Уроки");
    tabs->addTab(materialsTab, "Материалы");
    tabs->addTab(testsTab, "Тесты");

    layout->addWidget(m_courseTitleLabel);
    layout->addWidget(m_courseDescriptionLabel);
    layout->addWidget(m_messageLabel);
    layout->addWidget(tabs);

    rootLayout->addWidget(pageCard);

    connect(m_lessonTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_lessonContentEdit, &QTextEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_lessonsList, &QListWidget::itemSelectionChanged, this, [this]() { updateActionState(); });
    connect(m_materialLessonCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
        refreshMaterialsList();
        updateActionState();
    });
    connect(m_materialsList, &QListWidget::itemSelectionChanged, this, [this]() { updateActionState(); });
    connect(m_materialsList, &QListWidget::itemSelectionChanged, this, [this]() { refreshMaterialPreview(); });
    connect(m_questionTestCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
        Q_UNUSED(index);
        refreshQuestionsList();
        updateActionState();
        if (selectedManagedTestId() >= 0) {
            emit testSelectedForQuestions(selectedManagedTestId());
        }
    });
    connect(m_testsList, &QListWidget::itemSelectionChanged, this, [this]() {
        QListWidgetItem *currentItem = m_testsList->currentItem();
        if (!currentItem) {
            updateActionState();
            return;
        }

        const int testId = currentItem->data(Qt::UserRole).toInt();
        if (testId < 0) {
            updateActionState();
            return;
        }

        for (const auto &test : std::as_const(m_tests)) {
            if (test.id == testId) {
                populateTestDraft(test);
                break;
            }
        }

        for (int i = 0; i < m_questionTestCombo->count(); ++i) {
            if (m_questionTestCombo->itemData(i).toInt() == testId) {
                m_questionTestCombo->setCurrentIndex(i);
                break;
            }
        }

        showMessage("Тест выбран. Ниже можно посмотреть вопросы или перейти в отдельный редактор.", false);
        updateActionState();
    });
    connect(m_questionsList, &QListWidget::itemSelectionChanged, this, [this]() {
        const int questionId = selectedQuestionId();
        if (questionId < 0) {
            updateActionState();
            return;
        }

        for (const auto &question : std::as_const(m_questions)) {
            if (question.id == questionId) {
                populateQuestionDraft(question);
                break;
            }
        }
        updateActionState();
    });
    connect(m_materialTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_materialContentEdit, &QTextEdit::textChanged, this, [this]() {
        if (!m_syncingMaterialContent && !m_materialContentEdit->toPlainText().startsWith("Встроенный файл:")) {
            m_embeddedMaterialPayload.clear();
        }
        updateActionState();
    });
    connect(m_testTitleEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_questionTextEdit, &QTextEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionOneEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionTwoEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionThreeEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionFourEdit, &QLineEdit::textChanged, this, [this]() { updateActionState(); });
    connect(m_optionOrderCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() { updateActionState(); });
    connect(m_moveOptionUpButton, &QPushButton::clicked, this, [this]() {
        const int index = m_optionOrderCombo->currentIndex();
        if (index <= 0) {
            return;
        }

        QStringList options = questionOptionTexts();
        options.swapItemsAt(index, index - 1);
        setQuestionOptionTexts(options);

        int correctIndex = m_correctOptionCombo->currentIndex();
        if (correctIndex == index) {
            m_correctOptionCombo->setCurrentIndex(index - 1);
        } else if (correctIndex == index - 1) {
            m_correctOptionCombo->setCurrentIndex(index);
        }

        m_optionOrderCombo->setCurrentIndex(index - 1);
        showMessage("Порядок вариантов обновлён. Сохраните вопрос, чтобы зафиксировать изменения.", false);
    });
    connect(m_moveOptionDownButton, &QPushButton::clicked, this, [this]() {
        const int index = m_optionOrderCombo->currentIndex();
        if (index < 0 || index >= 3) {
            return;
        }

        QStringList options = questionOptionTexts();
        options.swapItemsAt(index, index + 1);
        setQuestionOptionTexts(options);

        int correctIndex = m_correctOptionCombo->currentIndex();
        if (correctIndex == index) {
            m_correctOptionCombo->setCurrentIndex(index + 1);
        } else if (correctIndex == index + 1) {
            m_correctOptionCombo->setCurrentIndex(index);
        }

        m_optionOrderCombo->setCurrentIndex(index + 1);
        showMessage("Порядок вариантов обновлён. Сохраните вопрос, чтобы зафиксировать изменения.", false);
    });

    connect(m_addLessonButton, &QPushButton::clicked, this, [this]() {
        QString title;
        QString content;
        if (!editLessonDialog(this, "Создать урок", {}, {}, &title, &content)) {
            return;
        }

        emit createLessonRequested(m_course.id, title, content);
    });
    connect(m_loadLessonButton, &QPushButton::clicked, this, [this]() {
        const int lessonId = selectedLessonListId();
        for (const auto& lesson : std::as_const(m_lessons)) {
            if (lesson.id == lessonId) {
                QString title;
                QString content;
                if (!editLessonDialog(this, "Редактировать урок", lesson.title, lesson.content, &title, &content)) {
                    return;
                }
                emit updateLessonRequested(lesson.id, title, content);
                return;
            }
        }
    });
    connect(m_updateLessonButton, &QPushButton::clicked, this, [this]() {
        emit updateLessonRequested(
            m_editingLessonId,
            m_lessonTitleEdit->text().trimmed(),
            m_lessonContentEdit->toPlainText().trimmed());
    });
    connect(m_deleteLessonButton, &QPushButton::clicked, this, [this]() {
        const int lessonId = selectedLessonListId();
        if (lessonId >= 0) {
            emit deleteLessonRequested(lessonId);
        }
    });
    connect(m_addMaterialButton, &QPushButton::clicked, this, [this]() {
        int lessonId = -1;
        QString title;
        QString type;
        QString content;
        if (!editMaterialDialog(this, "Создать материал", m_lessons, selectedLessonId(), {}, "text", {}, &lessonId, &title, &type, &content)) {
            return;
        }

        emit createMaterialRequested(lessonId, title, type, content);
    });
    connect(m_pickMaterialFileButton, &QPushButton::clicked, this, [this]() {
        attachLocalMaterialFile();
    });
    connect(m_loadMaterialButton, &QPushButton::clicked, this, [this]() {
        const int materialId = selectedMaterialId();
        for (const auto& material : std::as_const(m_materials)) {
            if (material.id == materialId) {
                int lessonId = material.lessonId;
                QString title;
                QString type;
                QString content;
                if (!editMaterialDialog(this, "Редактировать материал", m_lessons, material.lessonId, material.title, material.type, material.content, &lessonId, &title, &type, &content)) {
                    return;
                }
                emit updateMaterialRequested(material.id, title, type, content);
                return;
            }
        }
    });
    connect(m_previewMaterialButton, &QPushButton::clicked, this, [this]() {
        const int materialId = selectedMaterialId();
        for (const MaterialData &material : std::as_const(m_materials)) {
            if (material.id != materialId) {
                continue;
            }

            QJsonObject payload;
            if (parseEmbeddedFileMaterial(material.content, &payload)) {
                emit materialOpenExternalRequested(material.id);
            } else if (material.type == "link" || material.type == "video") {
                emit materialLinkOpenRequested(material.content);
            } else {
                emit materialTextPreviewRequested(material.title, material.content);
            }
            return;
        }
    });
    connect(m_openMaterialButton, &QPushButton::clicked, this, [this]() {
        const int materialId = selectedMaterialId();
        for (const MaterialData &material : std::as_const(m_materials)) {
            if (material.id != materialId) {
                continue;
            }

            if (material.type == "link" || material.type == "video") {
                emit materialLinkOpenRequested(material.content);
            } else if (parseEmbeddedFileMaterial(material.content, nullptr)) {
                emit materialOpenExternalRequested(material.id);
            } else {
                emit materialTextPreviewRequested(material.title, material.content);
            }
            return;
        }
    });
    connect(m_downloadMaterialButton, &QPushButton::clicked, this, [this]() {
        if (selectedMaterialId() >= 0) {
            emit materialDownloadRequested(selectedMaterialId());
        }
    });
    connect(m_updateMaterialButton, &QPushButton::clicked, this, [this]() {
        emit updateMaterialRequested(
            m_editingMaterialId,
            m_materialTitleEdit->text().trimmed(),
            m_materialTypeCombo->currentText(),
            materialContentForSubmit());
    });
    connect(m_deleteMaterialButton, &QPushButton::clicked, this, [this]() {
        const int materialId = selectedMaterialId();
        if (materialId >= 0) {
            emit deleteMaterialRequested(materialId);
        }
    });
    connect(m_addTestButton, &QPushButton::clicked, this, [this]() {
        QString title;
        QString status;
        QString deadlineAt;
        int maxAttempts = 0;
        int timeLimitMinutes = 30;
        if (!editTestDialog(this, "Создать тест", {}, "active", {}, 0, 30, &title, &status, &deadlineAt, &maxAttempts, &timeLimitMinutes)) {
            return;
        }

        emit createTestRequested(m_course.id, title, status, deadlineAt, maxAttempts, timeLimitMinutes);
    });
    connect(m_loadTestButton, &QPushButton::clicked, this, [this]() {
        const int testId = selectedManagedTestId();
        for (const auto& test : std::as_const(m_tests)) {
            if (test.id == testId) {
                QString title;
                QString status;
                QString deadlineAt;
                int maxAttempts = test.maxAttempts;
                int timeLimitMinutes = test.timeLimitMinutes;
                if (!editTestDialog(this, "Редактировать тест", test.title, test.status, test.deadlineAt, test.maxAttempts, test.timeLimitMinutes, &title, &status, &deadlineAt, &maxAttempts, &timeLimitMinutes)) {
                    return;
                }
                emit updateTestRequested(test.id, title, status, deadlineAt, maxAttempts, timeLimitMinutes);
                return;
            }
        }
    });
    connect(m_updateTestButton, &QPushButton::clicked, this, [this]() {
        emit updateTestRequested(m_editingTestId, m_testTitleEdit->text().trimmed(), "active", {}, 0, 30);
    });
    connect(m_deleteTestButton, &QPushButton::clicked, this, [this]() {
        const int testId = selectedManagedTestId();
        if (testId >= 0) {
            emit deleteTestRequested(testId);
        }
    });
    connect(openEditorButton, &QPushButton::clicked, this, &TeacherCourseBuilderPage::openDedicatedTestEditorRequested);
    connect(openQuestionEditorButton, &QPushButton::clicked, this, &TeacherCourseBuilderPage::openDedicatedTestEditorRequested);
    connect(m_addQuestionButton, &QPushButton::clicked, this, [this]() {
        QStringList options{
            m_optionOneEdit->text().trimmed(),
            m_optionTwoEdit->text().trimmed(),
            m_optionThreeEdit->text().trimmed(),
            m_optionFourEdit->text().trimmed()
        };

        emit createQuestionRequested(
            selectedManagedTestId(),
            m_questionTextEdit->toPlainText().trimmed(),
            options,
            m_correctOptionCombo->currentIndex());
    });
    connect(m_loadQuestionButton, &QPushButton::clicked, this, [this]() {
        const int questionId = selectedQuestionId();
        for (const auto& question : std::as_const(m_questions)) {
            if (question.id == questionId) {
                populateQuestionDraft(question);
                showMessage("Вопрос загружен в форму для редактирования.", false);
                return;
            }
        }
    });
    connect(m_updateQuestionButton, &QPushButton::clicked, this, [this]() {
        QStringList options{
            m_optionOneEdit->text().trimmed(),
            m_optionTwoEdit->text().trimmed(),
            m_optionThreeEdit->text().trimmed(),
            m_optionFourEdit->text().trimmed()
        };
        emit updateQuestionRequested(
            m_editingQuestionId,
            m_questionTextEdit->toPlainText().trimmed(),
            options,
            m_correctOptionCombo->currentIndex());
    });
    connect(m_deleteQuestionButton, &QPushButton::clicked, this, [this]() {
        emit deleteQuestionRequested(m_editingQuestionId);
    });

    clearBuilder();
}

void TeacherCourseBuilderPage::clearBuilder()
{
    m_course = CourseData{};
    m_lessons.clear();
    m_materials.clear();
    m_tests.clear();
    m_questions.clear();
    m_editingLessonId = -1;
    m_editingMaterialId = -1;
    m_editingTestId = -1;
    m_editingQuestionId = -1;

    m_courseTitleLabel->setText("Конструктор курса");
    m_courseDescriptionLabel->setText("Выберите курс во вкладке \"Мои курсы\", чтобы начать оформление структуры курса.");
    showMessage("Сейчас конструктор ждёт выбранный курс.", false);

    clearLessonDraft();
    clearMaterialDraft();
    clearTestDraft();
    clearQuestionDraft();
    refreshOverview();
    refreshLessonsList();
    refreshLessonSelector();
    refreshMaterialsList();
    refreshTestSelector();
    refreshTestsList();
    refreshQuestionsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setCourse(const CourseData &course)
{
    m_course = course;
    m_courseTitleLabel->setText(course.title.isEmpty() ? "Курс без названия" : course.title);
    m_courseDescriptionLabel->setText(
        course.description.isEmpty()
            ? "У курса пока нет описания. Структуру можно добавить ниже."
            : course.description);
    refreshOverview();
    updateActionState();
}

void TeacherCourseBuilderPage::setLessons(const QVector<LessonData> &lessons)
{
    m_lessons = lessons;
    refreshOverview();
    refreshLessonsList();
    refreshLessonSelector();
    refreshMaterialsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setMaterials(const QVector<MaterialData> &materials)
{
    m_materials = materials;
    refreshOverview();
    refreshMaterialsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setTests(const QVector<TestData> &tests)
{
    m_tests = tests;
    refreshOverview();
    refreshTestSelector();
    refreshTestsList();
    updateActionState();
}

void TeacherCourseBuilderPage::setQuestions(const QVector<QuestionData> &questions)
{
    m_questions = questions;
    refreshQuestionsList();
    updateActionState();
}

void TeacherCourseBuilderPage::showMessage(const QString &message, bool error)
{
    m_messageLabel->setText(message);
    m_messageLabel->setStyleSheet(
        error
            ? "color: #b91c1c; font-size: 13px; font-weight: 500;"
            : "color: #0f766e; font-size: 13px; font-weight: 500;");
}

void TeacherCourseBuilderPage::clearLessonDraft()
{
    m_editingLessonId = -1;
    m_lessonTitleEdit->clear();
    m_lessonContentEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::clearMaterialDraft()
{
    m_editingMaterialId = -1;
    m_embeddedMaterialPayload.clear();
    m_materialTitleEdit->clear();
    m_materialContentEdit->clear();
    m_materialTypeCombo->setCurrentIndex(0);
    m_materialPreviewEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::clearTestDraft()
{
    m_editingTestId = -1;
    m_testTitleEdit->clear();
    updateActionState();
}

void TeacherCourseBuilderPage::clearQuestionDraft()
{
    m_editingQuestionId = -1;
    m_questionTextEdit->clear();
    m_optionOneEdit->clear();
    m_optionTwoEdit->clear();
    m_optionThreeEdit->clear();
    m_optionFourEdit->clear();
    m_correctOptionCombo->setCurrentIndex(0);
    m_optionOrderCombo->setCurrentIndex(0);
    updateActionState();
}

int TeacherCourseBuilderPage::currentManagedTestId() const
{
    return selectedManagedTestId();
}

void TeacherCourseBuilderPage::populateTestDraft(const TestData &test)
{
    m_editingTestId = test.id;
    m_testTitleEdit->setText(test.title);
    updateActionState();
}

void TeacherCourseBuilderPage::populateQuestionDraft(const QuestionData &question)
{
    m_editingQuestionId = question.id;
    m_questionTextEdit->setText(question.text);
    m_optionOneEdit->setText(question.options.size() > 0 ? question.options[0].text : QString());
    m_optionTwoEdit->setText(question.options.size() > 1 ? question.options[1].text : QString());
    m_optionThreeEdit->setText(question.options.size() > 2 ? question.options[2].text : QString());
    m_optionFourEdit->setText(question.options.size() > 3 ? question.options[3].text : QString());

    int correctIndex = 0;
    for (int i = 0; i < question.options.size(); ++i) {
        if (question.options[i].id == question.correctAnswerId) {
            correctIndex = i;
            break;
        }
    }
    m_correctOptionCombo->setCurrentIndex(correctIndex);
    updateActionState();
}

void TeacherCourseBuilderPage::populateLessonDraft(const LessonData &lesson)
{
    m_editingLessonId = lesson.id;
    m_lessonTitleEdit->setText(lesson.title);
    m_lessonContentEdit->setText(lesson.content);
    updateActionState();
}

void TeacherCourseBuilderPage::populateMaterialDraft(const MaterialData &material)
{
    m_editingMaterialId = material.id;
    for (int i = 0; i < m_materialLessonCombo->count(); ++i) {
        if (m_materialLessonCombo->itemData(i).toInt() == material.lessonId) {
            m_materialLessonCombo->setCurrentIndex(i);
            break;
        }
    }
    m_materialTitleEdit->setText(material.title);
    const int typeIndex = m_materialTypeCombo->findText(material.type);
    m_materialTypeCombo->setCurrentIndex(typeIndex >= 0 ? typeIndex : 0);
    QJsonObject filePayload;
    if (parseEmbeddedFileMaterial(material.content, &filePayload)) {
        m_embeddedMaterialPayload = material.content;
        m_syncingMaterialContent = true;
        m_materialContentEdit->setText(QString("Встроенный файл: %1\nРазмер: %2 КБ")
            .arg(filePayload.value("fileName").toString(material.title))
            .arg((filePayload.value("size").toInt(0) + 1023) / 1024));
        m_syncingMaterialContent = false;
    } else {
        m_embeddedMaterialPayload.clear();
        m_materialContentEdit->setText(material.content);
    }
    refreshMaterialPreview();
    updateActionState();
}

void TeacherCourseBuilderPage::refreshOverview()
{
    m_lessonsSummaryLabel->setText(QString::number(m_lessons.size()));
    m_materialsSummaryLabel->setText(QString::number(m_materials.size()));
    m_testsSummaryLabel->setText(QString::number(m_tests.size()));

    const bool hasCourse = m_course.id >= 0;
    const bool lessonsReady = !m_lessons.isEmpty();
    const bool materialsReady = !m_materials.isEmpty();
    const bool testsReady = !m_tests.isEmpty();
    const bool builderReady = lessonsReady && materialsReady && testsReady;

    if (!hasCourse) {
        m_stageChecklistLabel->setText(
            "Шаг 1. Выбрать курс во вкладке \"Мои курсы\".\n"
            "Шаг 2. Добавление уроков и привязка материалов.\n"
            "Шаг 3. Настройка тестов завершает базовую проверку структуры курса.");
        return;
    }

    m_stageChecklistLabel->setText(QString(
        "Шаг 1. Уроки — %1 (%2)\n"
        "Шаг 2. Материалы — %3 (%4)\n"
        "Шаг 3. Тесты — %5 (%6)\n"
        "Итог: %7")
        .arg(statusWord(lessonsReady))
        .arg(lessonsReady ? QString("создано %1").arg(m_lessons.size()) : QString("сначала нужен первый урок"))
        .arg(statusWord(materialsReady))
        .arg(materialsReady ? QString("добавлено %1").arg(m_materials.size()) : QString("нужно привязать материалы к урокам"))
        .arg(statusWord(testsReady))
        .arg(testsReady ? QString("создано %1").arg(m_tests.size()) : QString("нужно настроить хотя бы один тест"))
        .arg(builderReady
            ? "структура курса уже выглядит полной, можно идти в студентов и аналитику."
            : "конструктор ещё не завершён: нужны недостающие блоки, чтобы курс стал рабочим."));
}

void TeacherCourseBuilderPage::refreshLessonsList()
{
    m_lessonsList->clear();

    if (m_lessons.isEmpty()) {
        appendBuilderCard(
            m_lessonsList,
            "Уроков пока нет",
            "Первый урок можно добавить через форму справа, и он сразу появится в структуре курса.");
        return;
    }

    for (const LessonData &lesson : std::as_const(m_lessons)) {
        appendBuilderCard(
            m_lessonsList,
            lesson.title,
            lesson.content.isEmpty() ? "Контент урока пока пуст" : compactBuilderPreview(lesson.content));
        m_lessonsList->item(m_lessonsList->count() - 1)->setData(Qt::UserRole, lesson.id);
    }
}

void TeacherCourseBuilderPage::refreshLessonSelector()
{
    const int currentLesson = selectedLessonId();

    m_materialLessonCombo->blockSignals(true);
    m_materialLessonCombo->clear();

    for (const LessonData &lesson : std::as_const(m_lessons)) {
        m_materialLessonCombo->addItem(lesson.title, lesson.id);
    }

    if (!m_lessons.isEmpty()) {
        int selectedIndex = 0;
        for (int i = 0; i < m_materialLessonCombo->count(); ++i) {
            if (m_materialLessonCombo->itemData(i).toInt() == currentLesson) {
                selectedIndex = i;
                break;
            }
        }
        m_materialLessonCombo->setCurrentIndex(selectedIndex);
    }

    m_materialLessonCombo->blockSignals(false);
}

void TeacherCourseBuilderPage::refreshMaterialsList()
{
    m_materialsList->clear();

    const int lessonId = selectedLessonId();
    if (lessonId < 0) {
        appendBuilderCard(
            m_materialsList,
            "Сначала нужен урок",
            "Материалы привязываются к урокам, поэтому сначала нужно создать хотя бы один урок.");
        return;
    }

    bool hasItems = false;
    for (const MaterialData &material : std::as_const(m_materials)) {
        if (material.lessonId != lessonId) {
            continue;
        }

        hasItems = true;

        QJsonObject filePayload;
        const QString subtitle = parseEmbeddedFileMaterial(material.content, &filePayload)
            ? QString("Файл: %1  •  Тип: %2  •  Размер: %3 КБ")
                .arg(filePayload.value("fileName").toString(material.title))
                .arg(material.type)
                .arg((filePayload.value("size").toInt(0) + 1023) / 1024)
            : compactBuilderPreview(material.content);

        appendBuilderCard(
            m_materialsList,
            QString("%1 (%2)").arg(material.title, material.type),
            subtitle);
        m_materialsList->item(m_materialsList->count() - 1)->setData(Qt::UserRole, material.id);
    }

    if (!hasItems) {
        appendBuilderCard(
            m_materialsList,
            "Материалов для этого урока пока нет",
            "Нужно выбрать тип материала и добавить первый текст, ссылку или видео.");
    }

    refreshMaterialPreview();
}

void TeacherCourseBuilderPage::refreshTestsList()
{
    m_testsList->clear();

    if (m_tests.isEmpty()) {
        appendBuilderCard(
            m_testsList,
            "Тестов пока нет",
            "Сначала нужно создать первый тест курса, чтобы позже перейти к редактору вопросов.");
        return;
    }

    for (const TestData &test : std::as_const(m_tests)) {
        const QString statusText = test.status == "closed"
            ? "закрыт"
            : test.available ? "активен" : "дедлайн истёк";
        const QString deadlineText = test.deadlineAt.isEmpty()
            ? "без дедлайна"
            : QString("дедлайн: %1").arg(test.deadlineAt);
        const QString attemptsText = test.maxAttempts == 0
            ? "попытки: без ограничения"
            : QString("попытки: %1").arg(test.maxAttempts);
        const QString timeText = QString("время: %1 мин.").arg(test.timeLimitMinutes);
        appendBuilderCard(
            m_testsList,
            test.title,
            QString("ID теста: %1  •  %2  •  %3  •  %4  •  %5")
                .arg(test.id)
                .arg(statusText, deadlineText, attemptsText, timeText));
        m_testsList->item(m_testsList->count() - 1)->setData(Qt::UserRole, test.id);
    }
}

void TeacherCourseBuilderPage::refreshTestSelector()
{
    const int currentTestId = selectedManagedTestId();

    m_questionTestCombo->blockSignals(true);
    m_questionTestCombo->clear();

    for (const TestData &test : std::as_const(m_tests)) {
        m_questionTestCombo->addItem(test.title, test.id);
    }

    if (!m_tests.isEmpty()) {
        int selectedIndex = 0;
        for (int i = 0; i < m_questionTestCombo->count(); ++i) {
            if (m_questionTestCombo->itemData(i).toInt() == currentTestId) {
                selectedIndex = i;
                break;
            }
        }
        m_questionTestCombo->setCurrentIndex(selectedIndex);
    }

    m_questionTestCombo->blockSignals(false);
}

void TeacherCourseBuilderPage::refreshQuestionsList()
{
    m_questionsList->clear();

    const int testId = selectedManagedTestId();
    if (testId < 0) {
        appendBuilderCard(
            m_questionsList,
            "Сначала нужен тест",
            "После этого нужно выбрать его в форме справа и начать добавлять вопросы.");
        return;
    }

    if (m_questions.isEmpty()) {
        appendBuilderCard(
            m_questionsList,
            "Вопросов пока нет",
            "Первый вопрос и четыре варианта ответа добавляются через форму справа.");
        return;
    }

    for (const QuestionData &question : std::as_const(m_questions)) {
        QString correctText = "Правильный ответ пока не определён";
        for (const auto &option : question.options) {
            if (option.id == question.correctAnswerId) {
                correctText = QString("Правильный ответ: %1").arg(option.text);
                break;
            }
        }

        appendBuilderCard(
            m_questionsList,
            question.text,
            correctText);
        m_questionsList->item(m_questionsList->count() - 1)->setData(Qt::UserRole, question.id);
    }
}

void TeacherCourseBuilderPage::refreshMaterialPreview()
{
    const int materialId = selectedMaterialId();
    if (materialId < 0) {
        QJsonObject draftPayload;
        if (parseEmbeddedFileMaterial(materialContentForSubmit(), &draftPayload)) {
            MaterialData draft;
            draft.title = m_materialTitleEdit->text();
            draft.type = m_materialTypeCombo->currentText();
            draft.content = materialContentForSubmit();
            m_materialPreviewEdit->setPlainText(materialPreviewText(draft));
            return;
        }

        m_materialPreviewEdit->setPlainText("Выберите материал в списке слева, чтобы увидеть его содержимое.");
        return;
    }

    for (const MaterialData &material : std::as_const(m_materials)) {
        if (material.id != materialId) {
            continue;
        }

        m_materialPreviewEdit->setPlainText(materialPreviewText(material));
        return;
    }

    m_materialPreviewEdit->setPlainText("Предпросмотр материала недоступен.");
}

void TeacherCourseBuilderPage::attachLocalMaterialFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        "Выбрать материал",
        QString(),
        "Учебные материалы (*.pdf *.doc *.docx *.txt *.md);;PDF (*.pdf);;Word (*.doc *.docx);;Текст (*.txt *.md);;Все файлы (*)");

    if (path.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        showMessage("Выбранный файл недоступен.", true);
        return;
    }

    if (fileInfo.size() > kMaxEmbeddedMaterialBytes) {
        showMessage("Файл слишком большой. Сейчас поддерживаются материалы до 5 МБ.", true);
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        showMessage("Не удалось открыть файл для чтения.", true);
        return;
    }

    const QByteArray bytes = file.readAll();
    const QString type = materialTypeFromSuffix(fileInfo.suffix());
    const QJsonObject payload{
        {"kind", "embedded-file"},
        {"fileName", fileInfo.fileName()},
        {"mimeType", materialMimeFromType(type)},
        {"size", static_cast<int>(bytes.size())},
        {"data", QString::fromLatin1(bytes.toBase64())}
    };

    const int typeIndex = m_materialTypeCombo->findText(type);
    m_materialTypeCombo->setCurrentIndex(typeIndex >= 0 ? typeIndex : m_materialTypeCombo->findText("file"));
    if (m_materialTitleEdit->text().trimmed().isEmpty()) {
        m_materialTitleEdit->setText(fileInfo.completeBaseName());
    }
    m_embeddedMaterialPayload = QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact));
    m_syncingMaterialContent = true;
    m_materialContentEdit->setPlainText(QString("Встроенный файл: %1\nРазмер: %2 КБ")
        .arg(fileInfo.fileName())
        .arg((bytes.size() + 1023) / 1024));
    m_syncingMaterialContent = false;
    m_materialPreviewEdit->setPlainText(materialPreviewText(MaterialData{
        -1,
        selectedLessonId(),
        m_materialTitleEdit->text().trimmed(),
        m_materialTypeCombo->currentText(),
        m_embeddedMaterialPayload
    }));
    showMessage(QString("Файл \"%1\" прикреплён к форме материала. Теперь можно сохранить материал.").arg(fileInfo.fileName()), false);
    updateActionState();
}

QString TeacherCourseBuilderPage::materialContentForSubmit() const
{
    if (!m_embeddedMaterialPayload.isEmpty()
        && m_materialContentEdit->toPlainText().startsWith("Встроенный файл:")) {
        return m_embeddedMaterialPayload;
    }

    return m_materialContentEdit->toPlainText().trimmed();
}

QStringList TeacherCourseBuilderPage::questionOptionTexts() const
{
    return {
        m_optionOneEdit->text(),
        m_optionTwoEdit->text(),
        m_optionThreeEdit->text(),
        m_optionFourEdit->text()
    };
}

void TeacherCourseBuilderPage::setQuestionOptionTexts(const QStringList &options)
{
    m_optionOneEdit->setText(options.value(0));
    m_optionTwoEdit->setText(options.value(1));
    m_optionThreeEdit->setText(options.value(2));
    m_optionFourEdit->setText(options.value(3));
}

int TeacherCourseBuilderPage::selectedLessonId() const
{
    if (m_materialLessonCombo->count() == 0) {
        return -1;
    }

    return m_materialLessonCombo->currentData().toInt();
}

int TeacherCourseBuilderPage::selectedLessonListId() const
{
    QListWidgetItem *item = m_lessonsList->currentItem();
    if (!item) {
        return -1;
    }

    const QVariant lessonId = item->data(Qt::UserRole);
    if (!lessonId.isValid()) {
        return -1;
    }

    return lessonId.toInt();
}

int TeacherCourseBuilderPage::selectedMaterialId() const
{
    QListWidgetItem *item = m_materialsList->currentItem();
    if (!item) {
        return -1;
    }

    const QVariant materialId = item->data(Qt::UserRole);
    if (!materialId.isValid()) {
        return -1;
    }

    return materialId.toInt();
}

int TeacherCourseBuilderPage::selectedManagedTestId() const
{
    if (m_questionTestCombo->count() == 0) {
        return -1;
    }

    return m_questionTestCombo->currentData().toInt();
}

int TeacherCourseBuilderPage::selectedQuestionId() const
{
    QListWidgetItem *item = m_questionsList->currentItem();
    if (!item) {
        return -1;
    }

    const QVariant questionId = item->data(Qt::UserRole);
    if (!questionId.isValid()) {
        return -1;
    }

    return questionId.toInt();
}

void TeacherCourseBuilderPage::updateActionState()
{
    const bool hasCourse = m_course.id >= 0;
    const bool hasLessons = !m_lessons.isEmpty();
    const bool hasTests = !m_tests.isEmpty();
    const bool hasSelectedLesson = selectedLessonId() >= 0;
    const bool hasSelectedTest = selectedManagedTestId() >= 0;
    const int currentLessonId = selectedLessonListId();
    const int currentMaterialId = selectedMaterialId();
    bool selectedMaterialIsFile = false;
    bool selectedMaterialIsLink = false;
    bool selectedMaterialIsVideo = false;
    for (const MaterialData &material : std::as_const(m_materials)) {
        if (material.id != currentMaterialId) {
            continue;
        }
        selectedMaterialIsFile = parseEmbeddedFileMaterial(material.content, nullptr);
        selectedMaterialIsLink = material.type == "link";
        selectedMaterialIsVideo = material.type == "video";
        break;
    }
    const bool materialFormEnabled = hasCourse && hasLessons;
    const bool questionFormEnabled = hasCourse && hasTests && hasSelectedTest;

    m_addLessonButton->setEnabled(hasCourse);
    m_loadLessonButton->setEnabled(currentLessonId >= 0);
    m_updateLessonButton->setEnabled(
        hasCourse
        && m_editingLessonId >= 0
        && !m_lessonTitleEdit->text().trimmed().isEmpty()
        && !m_lessonContentEdit->toPlainText().trimmed().isEmpty());
    m_deleteLessonButton->setEnabled(currentLessonId >= 0);

    m_addMaterialButton->setEnabled(hasCourse && hasLessons && hasSelectedLesson);
    m_loadMaterialButton->setEnabled(currentMaterialId >= 0);
    m_pickMaterialFileButton->setEnabled(materialFormEnabled && hasSelectedLesson);
    m_previewMaterialButton->setEnabled(currentMaterialId >= 0);
    m_previewMaterialButton->setText(selectedMaterialIsFile
        ? "Открыть во внешнем приложении"
        : selectedMaterialIsVideo
            ? "Открыть видео"
            : selectedMaterialIsLink
                ? "Открыть ссылку"
                : "Предпросмотр");
    m_openMaterialButton->setEnabled(currentMaterialId >= 0);
    m_openMaterialButton->setText(selectedMaterialIsFile
        ? "Открыть файл"
        : selectedMaterialIsVideo
            ? "Открыть видео"
            : selectedMaterialIsLink
                ? "Открыть ссылку"
                : "Открыть текст");
    m_downloadMaterialButton->setEnabled(currentMaterialId >= 0 && selectedMaterialIsFile);
    m_updateMaterialButton->setEnabled(
        hasCourse
        && hasLessons
        && m_editingMaterialId >= 0
        && hasSelectedLesson
        && !m_materialTitleEdit->text().trimmed().isEmpty()
        && !materialContentForSubmit().trimmed().isEmpty());
    m_deleteMaterialButton->setEnabled(currentMaterialId >= 0);

    m_addTestButton->setEnabled(hasCourse);
    m_loadTestButton->setEnabled(hasTests && selectedManagedTestId() >= 0);
    m_updateTestButton->setEnabled(
        hasCourse
        && m_editingTestId >= 0
        && !m_testTitleEdit->text().trimmed().isEmpty());
    m_deleteTestButton->setEnabled(hasTests && selectedManagedTestId() >= 0);

    m_addQuestionButton->setEnabled(
        hasCourse
        && hasTests
        && hasSelectedTest
        && !m_questionTextEdit->toPlainText().trimmed().isEmpty()
        && !m_optionOneEdit->text().trimmed().isEmpty()
        && !m_optionTwoEdit->text().trimmed().isEmpty()
        && !m_optionThreeEdit->text().trimmed().isEmpty()
        && !m_optionFourEdit->text().trimmed().isEmpty());
    m_loadQuestionButton->setEnabled(selectedQuestionId() >= 0);
    m_updateQuestionButton->setEnabled(
        hasCourse
        && m_editingQuestionId >= 0
        && hasTests
        && hasSelectedTest
        && !m_questionTextEdit->toPlainText().trimmed().isEmpty()
        && !m_optionOneEdit->text().trimmed().isEmpty()
        && !m_optionTwoEdit->text().trimmed().isEmpty()
        && !m_optionThreeEdit->text().trimmed().isEmpty()
        && !m_optionFourEdit->text().trimmed().isEmpty());
    m_deleteQuestionButton->setEnabled(m_editingQuestionId >= 0);
    m_moveOptionUpButton->setEnabled(questionFormEnabled && m_optionOrderCombo->currentIndex() > 0);
    m_moveOptionDownButton->setEnabled(questionFormEnabled && m_optionOrderCombo->currentIndex() >= 0 && m_optionOrderCombo->currentIndex() < 3);

    m_materialLessonCombo->setEnabled(materialFormEnabled);
    m_materialTitleEdit->setEnabled(materialFormEnabled);
    m_materialTypeCombo->setEnabled(materialFormEnabled);
    m_materialContentEdit->setEnabled(materialFormEnabled);
    m_materialPreviewEdit->setEnabled(true);
    m_materialsGuardLabel->setVisible(!materialFormEnabled || !hasSelectedLesson);
    if (!hasCourse) {
        m_materialsGuardLabel->setText("Сначала нужно выбрать курс, а затем перейти к наполнению уроков материалами.");
    } else if (!hasLessons) {
        m_materialsGuardLabel->setText("Сначала нужно создать хотя бы один урок. После этого здесь станет доступно добавление материалов.");
    } else if (!hasSelectedLesson) {
        m_materialsGuardLabel->setText("Выберите урок в выпадающем списке, чтобы привязать к нему материал.");
    }

    m_questionTestCombo->setEnabled(hasCourse && hasTests);
    m_questionTextEdit->setEnabled(questionFormEnabled);
    m_optionOneEdit->setEnabled(questionFormEnabled);
    m_optionTwoEdit->setEnabled(questionFormEnabled);
    m_optionThreeEdit->setEnabled(questionFormEnabled);
    m_optionFourEdit->setEnabled(questionFormEnabled);
    m_correctOptionCombo->setEnabled(questionFormEnabled);
    m_optionOrderCombo->setEnabled(questionFormEnabled);
    m_questionsGuardLabel->setVisible(!questionFormEnabled);
    if (!hasCourse) {
        m_questionsGuardLabel->setText("Сначала нужно выбрать курс, затем настроить тесты и только после этого перейти к вопросам.");
    } else if (!hasTests) {
        m_questionsGuardLabel->setText("Сначала нужно создать хотя бы один тест. После этого здесь можно будет собирать вопросы и правильные ответы.");
    } else if (!hasSelectedTest) {
        m_questionsGuardLabel->setText("Выберите тест в списке или в выпадающем поле, чтобы начать наполнять его вопросами.");
    }

    if (!hasCourse) {
        m_questionEditorSummaryLabel->setText("Сначала нужно выбрать курс. После этого здесь появится краткая сводка по выбранному тесту и переход к отдельному редактору.");
    } else if (!hasTests) {
        m_questionEditorSummaryLabel->setText("У курса пока нет тестов. После создания хотя бы одного теста в верхнем блоке можно будет переходить к вопросам.");
    } else if (!hasSelectedTest) {
        m_questionEditorSummaryLabel->setText("Выберите тест в списке слева. Здесь появится краткая сводка, а полное редактирование вопросов откроется в отдельном окне.");
    } else {
        const int questionCount = m_questions.size();
        m_questionEditorSummaryLabel->setText(
            questionCount == 0
                ? "У выбранного теста пока нет вопросов. Отдельный редактор поможет создать первый вопрос."
                : QString("У выбранного теста сейчас %1 вопросов. В builder оставляем только обзор, а глубокое редактирование ведём в отдельном редакторе.")
                    .arg(questionCount));
    }
}
