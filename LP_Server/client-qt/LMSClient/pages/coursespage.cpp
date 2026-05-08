#include "coursespage.h"

#include "../widgets/coursecard.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

CoursesPage::CoursesPage(QWidget *parent)
    : QWidget(parent)
{
    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(16);

    auto *pageCard = new QFrame(this);
    pageCard->setObjectName("pageCard");

    auto *pageLayout = new QVBoxLayout(pageCard);
    pageLayout->setContentsMargins(24, 24, 24, 24);
    pageLayout->setSpacing(14);

    m_titleLabel = new QLabel("Каталог курсов", pageCard);
    m_titleLabel->setObjectName("sectionTitleLabel");

    m_hintLabel = new QLabel(
        "Курсы подгружаются автоматически. Нажми на карточку курса, чтобы открыть его внутреннюю страницу.",
        pageCard);
    m_hintLabel->setObjectName("sectionHintLabel");
    m_hintLabel->setWordWrap(true);

    auto createStatCard = [pageCard](const QString &title, QLabel **titleOut, QLabel **valueOut) {
        auto *card = new QFrame(pageCard);
        card->setObjectName("moduleCard");
        card->setMinimumHeight(136);
        card->setMaximumHeight(156);

        auto *layout = new QVBoxLayout(card);
        layout->setContentsMargins(18, 18, 18, 18);
        layout->setSpacing(8);

        *titleOut = new QLabel(title, card);
        (*titleOut)->setObjectName("moduleTitleLabel");

        *valueOut = new QLabel("0", card);
        (*valueOut)->setObjectName("courseDetailTitleLabel");

        layout->addWidget(*titleOut);
        layout->addWidget(*valueOut);
        layout->addStretch();
        return card;
    };

    auto *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(14);
    statsLayout->addWidget(createStatCard("Курсы", &m_countTitleLabel, &m_countValueLabel));
    statsLayout->addWidget(createStatCard("Фокус", &m_focusTitleLabel, &m_focusValueLabel));

    m_infoLabel = new QLabel(pageCard);
    m_infoLabel->setObjectName("sectionHintLabel");
    m_infoLabel->setWordWrap(true);

    auto *scrollArea = new QScrollArea(pageCard);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setObjectName("coursesScrollArea");
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QWidget#coursesCardsContainer { background: #f8fbff; }");

    m_cardsContainer = new QWidget(scrollArea);
    m_cardsContainer->setObjectName("coursesCardsContainer");
    m_cardsLayout = new QVBoxLayout(m_cardsContainer);
    m_cardsLayout->setContentsMargins(0, 0, 0, 0);
    m_cardsLayout->setSpacing(14);
    m_cardsLayout->addStretch();

    scrollArea->setWidget(m_cardsContainer);

    pageLayout->addWidget(m_titleLabel);
    pageLayout->addWidget(m_hintLabel);
    pageLayout->addLayout(statsLayout);
    pageLayout->addWidget(m_infoLabel);
    pageLayout->addWidget(scrollArea);

    rootLayout->addWidget(pageCard);

    setRoleMode(m_role);
}

void CoursesPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher") {
        m_titleLabel->setText("Мои курсы");
        m_hintLabel->setText(
            "Здесь отображаются курсы, где ты преподаватель. Открой курс, чтобы перейти к его внутренней структуре.");
        m_countTitleLabel->setText("Мои курсы");
        m_focusTitleLabel->setText("Режим");
        m_focusValueLabel->setText(m_coursesCount == 0 ? "Старт" : "Builder");
        m_infoLabel->setText(
            m_coursesCount == 0
                ? "Сначала создай первый курс, после этого он появится здесь и его можно будет открыть или развивать в конструкторе."
                : "Открой курс для teacher-view или переходи в конструктор, чтобы редактировать уроки, материалы и тесты.");
    } else {
        m_titleLabel->setText("Каталог курсов");
        m_hintLabel->setText(
            "Курсы подгружаются автоматически. Нажми на карточку курса, чтобы открыть его внутреннюю страницу.");
        m_countTitleLabel->setText("Доступно курсов");
        m_focusTitleLabel->setText("Что дальше");
        m_focusValueLabel->setText(m_coursesCount == 0 ? "Ожидание" : "Учиться");
        m_infoLabel->setText(
            m_coursesCount == 0
                ? "Когда курсы станут доступны, они появятся здесь. После записи можно переходить к урокам, материалам и тестам."
                : "Сначала открой курс и изучи материалы, затем переходи к тестам и отслеживай результат в отдельной вкладке.");
    }

    m_countValueLabel->setText(QString::number(m_coursesCount));
}

void CoursesPage::setCourses(const QVector<CourseData> &courses)
{
    m_coursesCount = courses.size();
    setRoleMode(m_role);
    clearCards();

    for (const auto &course : courses) {
        auto *card = new CourseCard(
            course.id,
            course.title,
            course.description,
            m_role != "Teacher",
            m_cardsContainer);
        QObject::connect(card, &CourseCard::openRequested, this, [this, course](int) {
            emit courseOpened(course);
        });
        QObject::connect(card, &CourseCard::enrollRequested, this, [this](int courseId) {
            emit enrollRequested(courseId);
        });
        QObject::connect(card, &CourseCard::builderRequested, this, [this, course](int) {
            emit courseBuilderRequested(course);
        });
        QObject::connect(card, &CourseCard::editRequested, this, [this, course](int) {
            emit courseEditRequested(course);
        });
        QObject::connect(card, &CourseCard::deleteRequested, this, [this, course](int) {
            emit courseDeleteRequested(course);
        });

        addCardWidget(card);
    }
}

void CoursesPage::showPlaceholder(const QString &title, const QString &message)
{
    m_coursesCount = 0;
    setRoleMode(m_role);
    clearCards();

    auto *card = new QFrame(m_cardsContainer);
    card->setObjectName("courseCard");

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("courseCardTitleLabel");

    auto *messageLabel = new QLabel(message, card);
    messageLabel->setObjectName("courseCardDescriptionLabel");
    messageLabel->setWordWrap(true);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    addCardWidget(card);
}

void CoursesPage::clearCards()
{
    while (m_cardsLayout->count() > 1) {
        QLayoutItem *item = m_cardsLayout->takeAt(0);
        if (!item) {
            break;
        }

        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }

        delete item;
    }
}

void CoursesPage::addCardWidget(QWidget *widget)
{
    m_cardsLayout->insertWidget(m_cardsLayout->count() - 1, widget);
}
