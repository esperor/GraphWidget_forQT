#pragma once

#include <QFont>
#include <QPoint>
#include <QPainterPath>
#include "GraphLib_global.h"

namespace GraphLib {

QFont GRAPHLIB_EXPORT standardFont(int size);

QPoint GRAPHLIB_EXPORT snap(const QPointF &position, short interval);

QPainterPath GRAPHLIB_EXPORT standardPath(const QPoint &origin, const QPoint &target, float zoomMult = 1.0f);

}
