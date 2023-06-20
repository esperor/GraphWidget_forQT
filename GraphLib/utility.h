#pragma once

#include <QFont>
#include <QPoint>
#include <QPainterPath>
#include <optional>

#include "GraphLib_global.h"

namespace GraphLib {

class BaseNode;
enum class PinDirection;
class TypedNode;

std::optional<QJsonObject> loadFile(const char* name);

QFont GRAPHLIB_EXPORT standardFont(int size);

QPoint GRAPHLIB_EXPORT snap(const QPointF &position, short interval);

QPainterPath GRAPHLIB_EXPORT standardPath(const QPoint &origin, const QPoint &target, float zoomMult = 1.0f);

namespace NodeFactoryModule {

QColor GRAPHLIB_EXPORT parseToColor(const QString &str);

}

}
