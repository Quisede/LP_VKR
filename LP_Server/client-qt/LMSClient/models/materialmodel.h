#pragma once

#include <QByteArray>
#include <QString>

struct MaterialData {
    int id = -1;
    int lessonId = -1;
    QString title;
    QString type;
    QString content;
};

struct MaterialFileData {
    int materialId = -1;
    QString fileName;
    QString mimeType;
    QByteArray bytes;
};
