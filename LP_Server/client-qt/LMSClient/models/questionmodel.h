#pragma once

#include <QString>
#include <QVector>

struct AnswerOptionData {
    int id = -1;
    QString text;
};

struct QuestionData {
    int id = -1;
    QString text;
    QVector<AnswerOptionData> options;
    int correctAnswerId = -1;
};
