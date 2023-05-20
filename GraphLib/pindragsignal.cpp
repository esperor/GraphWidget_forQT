
#include "pindragsignal.h"

namespace GraphLib {

PinDragSignal::PinDragSignal() {}
PinDragSignal::PinDragSignal(PinData source, PinDragSignalType type)
    : _source{ source }, _type{ type } {}

PinDragMoveSignal::PinDragMoveSignal() {}
PinDragMoveSignal::PinDragMoveSignal(PinData source, PinDragSignalType type, QPointF mousePosition)
    : PinDragSignal(source, type), _mousePosition{ mousePosition } {}

}
