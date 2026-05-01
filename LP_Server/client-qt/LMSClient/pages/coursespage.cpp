#include "coursespage.h"

#include "../widgets/coursecard.h"

#include <QFrame>
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
    pageLayout->addWidget(scrollArea);

    rootLayout->addWidget(pageCard);
}

void CoursesPage::setRoleMode(const QString &role)
{
    m_role = role;

    if (role == "Teacher") {
        m_titleLabel->setText("Мои курсы");
        m_hintLabel->setText(
            "Здесь отображаются курсы, где ты преподаватель. Открой курс, чтобы перейти к его внутренней структуре.");
    } else {
        m_titleLabel->setText("Каталог курсов");
        m_hintLabel->setText(
            "Курсы подгружаются автоматически. Нажми на карточку курса, чтобы открыть его внутреннюю страницу.");
    }
}

void CoursesPage::setCourses(const QVector<CourseData> &courses)
{
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

        addCardWidget(card);
    }
}

void CoursesPage::showPlaceholder(const QString &title, const QString &message)
{
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
