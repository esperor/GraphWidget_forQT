#pragma once

#include <QJsonValue>

#include "GraphLib_global.h"

namespace GraphLib {

class TypedNode;
class Canvas;
enum class PinDirection;

namespace NodeFactory {

TypedNode GRAPHLIB_EXPORT *getNodeOfType(int typeID, Canvas *canvas);

void GRAPHLIB_EXPORT addPinsToNodeByJsonValue(const QJsonValue &val, TypedNode *node, PinDirection direction);

QColor GRAPHLIB_EXPORT parseToColor(const QString &str);

class GRAPHLIB_EXPORT NodeFactory
{
public:
    NodeFactory();
};

}

}
