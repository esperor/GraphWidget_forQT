
#pragma once

#include <QString>
#include <QColor>
#include <QByteArray>
#include <QtDebug>
#include "GraphLib_global.h"

namespace GraphLib {
enum class PinDirection;
class AbstractPin;

struct GRAPHLIB_EXPORT PinData
{
public:
    PinData() {}
    PinData(const PinData &other);
    PinData(PinDirection _direction, int _nodeID, int _pinID, const QString &_text, const QColor &_color);
    PinData(const AbstractPin *pin);
    virtual ~PinData();

    QByteArray toByteArray();
    static PinData fromByteArray(const QByteArray &byteArray);

    PinDirection pinDirection;
    int nodeID;
    int pinID;
    QString pinText;
    QColor color;
};

QDebug GRAPHLIB_EXPORT &operator<<(QDebug &debug, const PinData &obj);

QDataStream GRAPHLIB_EXPORT &operator<<(QDataStream &out, const PinData &obj);

bool GRAPHLIB_EXPORT operator<(const PinData &first, const PinData &second);
bool GRAPHLIB_EXPORT operator>(const PinData &first, const PinData &second);
bool GRAPHLIB_EXPORT operator==(const PinData &first, const PinData &second);

}

Q_DECLARE_METATYPE(GraphLib::PinData)
