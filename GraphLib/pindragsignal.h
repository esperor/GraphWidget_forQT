#pragma once

#include "GraphLib_global.h"
#include "pindata.h"
#include <QPointF>

namespace GraphLib {

enum class PinDragSignalType
{
    Enter,
    Leave,
    Start,
    End
};

struct GRAPHLIB_EXPORT PinDragSignal
{
public:
    PinDragSignal();
    PinDragSignal(PinData source, PinDragSignalType type);

    PinData &source() { return _source; }
    PinDragSignalType &type() { return _type; }

private:
    PinData _source;
    PinDragSignalType _type;
};

struct GRAPHLIB_EXPORT PinDragMoveSignal : public PinDragSignal
{
public:
    PinDragMoveSignal();
    PinDragMoveSignal(PinData source, PinDragSignalType type, QPointF mousePosition);

    QPointF &mousePosition() { return _mousePosition; }

private:
    QPointF _mousePosition;
};

}
