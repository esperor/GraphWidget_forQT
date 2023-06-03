#include <QPen>
#include <QRect>
#include <QPalette>
#include <algorithm>
#include <QMimeData>
#include <QLinearGradient>
#include <QPainterPath>
#include <cmath>

#include "canvas.h"
#include "utility.h"
#include "constants.h"
#include "typednode.h"
#include "TypeManagers/nodetypemanager.h"

namespace GraphLib {

Canvas::Canvas(QWidget *parent)
    : QWidget{ parent }
    , _painter{ new QPainter() }
    , _dotPaintGap{ 40 }
    , _bIsMouseDown{ false }
    , _draggedPin{ std::nullopt }
    , _draggedPinTargetInfo{ std::nullopt }
    , _draggedPinTarget{ QPoint() }
    , _offset{ QPointF(0, 0) }
    , _lastMouseDownPosition{ QPointF() }
    , _mousePosition{ QPointF(0, 0) }
    , _zoom{ -4 }
    , _accumulativeZoomDelta{ 0 }
    , _snappingInterval{ 20 }
    , _bIsSnappingEnabled{ true }
    , _nodes{ QVector<QSharedPointer<BaseNode>>() }
    , _connectedPins{ QMultiMap<PinData, PinData>() }
{
    setMouseTracking(true);
    setAutoFillBackground(true);

    QPalette palette(c_paletteDefaultColor);
    this->setPalette(palette);
    setAcceptDrops(true);

    QString path = "./../../GraphSubdirs/GraphTests/test_files/";
    QString pins = "pins.json", nodes = "nodes.json";

    NodeTypeManager::loadTypes(path + nodes);
    //PinTypeManager::loadTypes(path + pins);

    _nfWidget = new NodeFactory::NodeFactoryWidget(this);
    _nfWidget->setFixedSize(150, 600);
    _nfWidget->show();
    _nfWidget->initTypes();


    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &Canvas::tick);
    _timer->start(30);

    addBaseNode(QPoint(-200, 0), "Node 1");
    addBaseNode(QPoint(200, 0), "Node 2");
    addTypedNode(QPoint(200, -200), 1);
}

Canvas::~Canvas()
{
    delete _painter; delete _timer; delete _nfWidget;
}

const QMap<short, float> Canvas::_zoomMultipliers =
{
    {   0, 2.0f  },
    {  -1, 1.75f  },
    {  -2, 1.5f  },
    {  -3, 1.25f  },
    {  -4, 1.0f },
    {  -5, 0.9f },
    {  -6, 0.8f  },
    {  -7, 0.7f  },
    {  -8, 0.6f  },
    {  -9, 0.5f  },
    { -10, 0.4f  },
    { -11, 0.3f  },
    { -12, 0.2f  },
    { -13, 0.1f  },
};



// ---------------------- GENERAL FUNCTIONS ---------------------------


void Canvas::moveCanvasOnPinDragNearEdge(QPointF mousePosition)
{
    auto lerp = [&](float actual, float max) {
        return std::lerp(0, c_standardPinDragEdgeCanvasMoveValue, actual / max);
    };

    QRect rect = this->rect();
    QRectF top = rect, bottom = rect, left = rect, right = rect;

    int percentOfHeight = rect.height() * c_percentOfCanvasSizeToConsiderNearEdge;
    int percentOfWidth = rect.width() * c_percentOfCanvasSizeToConsiderNearEdge;

    top.setBottom(rect.top() + percentOfHeight);
    bottom.setTop(rect.bottom() - percentOfHeight);
    left.setRight(rect.left() + percentOfWidth);
    right.setLeft(rect.right() - percentOfWidth);


    if (top.contains(mousePosition))
        moveViewUp(lerp(top.bottom() - mousePosition.y(), top.height()));

    if (bottom.contains(mousePosition))
        moveViewDown(lerp(mousePosition.y() - bottom.top(), bottom.height()));

    if (left.contains(mousePosition))
        moveViewLeft(lerp(left.right() - mousePosition.x(), left.width()));

    if (right.contains(mousePosition))
        moveViewRight(lerp(mousePosition.x() - right.left(), right.width()));
}

void Canvas::moveCanvas(QPointF offset) { _offset -= offset / _zoomMultipliers[_zoom]; }
void Canvas::moveView(QVector2D vector) { moveCanvas(QPointF(-vector.x(), -vector.y())); }
void Canvas::moveViewUp(float by) { moveCanvas(QPointF(0, by)); }
void Canvas::moveViewDown(float by) { moveCanvas(QPointF(0, -by)); }
void Canvas::moveViewLeft(float by) { moveCanvas(QPointF(by, 0)); }
void Canvas::moveViewRight(float by) { moveCanvas(QPointF(-by, 0)); }

QPointF Canvas::mapToCanvas(QPointF point) const
{
    return (point - this->rect().center()) / _zoomMultipliers[_zoom] + _offset;
}

QPoint Canvas::mapToCanvas(QPoint point) const
{
    return ((point - this->rect().center()) / _zoomMultipliers[_zoom] + _offset.toPoint());
}

void Canvas::zoom(int times, QPointF where)
{
    if (times == 0) return;
    if (where.x() < 0 || where.y() < 0) where = _mousePosition;

    QPointF initialWhereOnCanvas = mapToCanvas(where);

    _zoom += times;

    if (times < 0)
        _zoom = std::max(_zoom, _zoomMultipliers.firstKey());
    else
        _zoom = std::min(_zoom, _zoomMultipliers.lastKey());

    QPointF whereOffset = mapToCanvas(where) - initialWhereOnCanvas;
    _offset -= whereOffset;
}

void Canvas::zoomIn(int times, QPointF where) { zoom(times, where); }

void Canvas::zoomOut(int times, QPointF where) { zoom(-times, where); }



// ---------------------------- SLOTS --------------------------------


void Canvas::tick()
{
    if (_draggedPin)
        moveCanvasOnPinDragNearEdge(_mousePosition);
}

void Canvas::onPinConnect(PinData outPin, PinData inPin)
{
    if (!_connectedPins.contains(outPin, inPin))
    {
        _connectedPins.insert(outPin, inPin);
        _nodes[outPin.nodeID]->setPinConnection(outPin.pinID, inPin);
        _nodes[inPin.nodeID]->setPinConnection(inPin.pinID, outPin);
    }
}

void Canvas::onPinDrag(PinDragSignal signal)
{
    switch (signal.type())
    {
    case PinDragSignalType::Enter:
    {
        _nodes[signal.source().nodeID]->setPinConnected(signal.source().pinID, true);
        if (_draggedPinTargetInfo)
            _draggedPinTargetInfo.value() = signal.source();
        break;
    }
    case PinDragSignalType::Leave:
    {
        _nodes[signal.source().nodeID]->setPinConnected(signal.source().pinID, false);
        if (_draggedPinTargetInfo)
            _draggedPinTargetInfo.value() = std::nullopt;
        break;
    }
    case PinDragSignalType::Start:
    {
        _draggedPin = signal.source();
        _draggedPinTargetInfo.emplace(std::nullopt);
        break;
    }
    case PinDragSignalType::End:
    {
        _draggedPin = std::nullopt;
        _draggedPinTargetInfo = std::nullopt;
        break;
    }
    default:;
    }
}

QWeakPointer<BaseNode> Canvas::addBaseNode(QPoint canvasPosition, QString name)
{
    int id = _nodes.length();
    BaseNode *node = new BaseNode(id, this, this);
    node->setCanvasPosition(canvasPosition);
    node->setName(name);

    return addNode(node);
}

QWeakPointer<BaseNode> Canvas::addNode(BaseNode *node)
{
    int id = _nodes.length();
    node->setID(id);

    _nodes.append(QSharedPointer<BaseNode>(node));
    _nodes[id]->show();

    connect(_nodes[id].get(), &BaseNode::signal_onPinDrag, this, &Canvas::onPinDrag);
    connect(_nodes[id].get(), &BaseNode::signal_onPinConnect, this, &Canvas::onPinConnect);

    return QWeakPointer<BaseNode>(_nodes[id]);
}

QWeakPointer<BaseNode> Canvas::addTypedNode(QPoint canvasPosition, int typeID)
{
    return addBaseNode(canvasPosition, NodeTypeManager::Types()[typeID].value("name").toString());
}


// --------------------------- EVENTS ----------------------------------


void Canvas::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::MouseButton::RightButton:
        _bIsMouseDown = true;
        _lastMouseDownPosition = event->position();
        this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
        break;
    default:;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    if (_bIsMouseDown)
    {
        QPointF offset = event->position() - _lastMouseDownPosition;
        moveCanvas(offset);

        _lastMouseDownPosition = event->position();
    }
    _mousePosition = event->position();
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    case Qt::MouseButton::RightButton:
        _bIsMouseDown = false;
        this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        break;
    default:;
    }
}

// accumulative zoom delta is used for mice with finer-resolution wheels
// https://doc.qt.io/qt-6/qwheelevent.html#angleDelta
void Canvas::wheelEvent(QWheelEvent *event)
{
    _accumulativeZoomDelta += event->angleDelta().y();
    if (_accumulativeZoomDelta >= 120)
    {
        short zoomSteps = qFloor(_accumulativeZoomDelta / 120);
        _accumulativeZoomDelta = _accumulativeZoomDelta % 120;

        zoomIn(zoomSteps, event->position());
    }
    else if (_accumulativeZoomDelta <= -120)
    {
        short zoomSteps = qFloor(_accumulativeZoomDelta / -120);
        _accumulativeZoomDelta = _accumulativeZoomDelta % 120;

        zoomOut(zoomSteps, event->position());
    }
}

void Canvas::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForNodeFactory))
    {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
        QByteArray byteArray = event->mimeData()->data(c_mimeFormatForNodeFactory);
        TypedNodeSpawnData data = TypedNodeSpawnData::fromByteArray(byteArray);

        qDebug() << _nodes;
        addTypedNode(mapToCanvas(event->position().toPoint()), data.typeID);
        qDebug() << _nodes;
    }
}

void Canvas::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForPinConnection) ||
        event->mimeData()->hasFormat(c_mimeFormatForNodeFactory))
    {
        event->setDropAction(Qt::CopyAction);
        event->acceptProposedAction();
    }
}

void Canvas::dragMoveEvent(QDragMoveEvent *event)
{
    if (_draggedPin && event->mimeData()->hasFormat(c_mimeFormatForPinConnection))
    {
        QPoint mousePos = event->position().toPoint();
        _draggedPinTarget = mousePos;
    }
    _mousePosition = event->position();
}


// -------------------------- PAINT -----------------------------------


void Canvas::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void Canvas::paint(QPainter *painter, QPaintEvent *event)
{
    QPen pen(Qt::SolidLine);
    pen.setColor(c_dotsColor);
    painter->setPen(pen);

    QRect rectangle = event->rect();

    QPoint center = rectangle.center();

    int halfWidth = center.x();
    int halfHeight = center.y();

    float zoomMult = _zoomMultipliers[_zoom];
    float dotPaintGapZoomedf = zoomMult * _dotPaintGap;

    // this line is responsible for increasing the dots' gap when it's too small
    int dotPaintGapZoomed = static_cast<int>(dotPaintGapZoomedf < _dotPaintGap / 2 ? dotPaintGapZoomedf * 6 : dotPaintGapZoomedf);

    auto calculateFirstDotCoord = [&](const int &half, const float &positionCoord) {
        return static_cast<int>( (half % dotPaintGapZoomed) - zoomMult * positionCoord ) % dotPaintGapZoomed;
    };

    int leftDotCoordX = calculateFirstDotCoord(halfWidth, _offset.x());
    int topDotCoordY = calculateFirstDotCoord(halfHeight, _offset.y());

    for (int x = leftDotCoordX; x < rectangle.width(); x += dotPaintGapZoomed)
    {
        for (int y = topDotCoordY; y < rectangle.height(); y += dotPaintGapZoomed)
        {
            painter->drawPoint(x, y);
        }
    }

    // manage NODES
    std::ranges::for_each(_nodes, [&](QSharedPointer<BaseNode> &node) {

        // this->rect()->center() is used instead of center purposefully
        // in order to fix flicking and lagging of the nodes (dk why it fixes the problem)
        const QPointF offset = zoomMult * (node->canvasPosition() - _offset) + this->rect().center();

        node->move(offset.toPoint());
        node->setFixedSize(node->normalSize() * zoomMult);
    });



    // draw PINS CONNECTIONS
    {

        pen.setWidth(c_pinConnectLineWidth * zoomMult);
        painter->setPen(pen);
        // manage currently dragged pin
        if (_draggedPin && _draggedPinTargetInfo)
        {
            bool bThereIsTargetPin = static_cast<bool>(_draggedPinTargetInfo.value());
            QPoint origin = _nodes[_draggedPin->nodeID]->getOutlineCoordinateForPinID(_draggedPin->pinID);
            QPoint target = bThereIsTargetPin
                                ? _nodes[_draggedPinTargetInfo.value()->nodeID]->
                getOutlineCoordinateForPinID(_draggedPinTargetInfo.value()->pinID)
                                : _draggedPinTarget;

            // origin is always either an out-pin or the cursor
            if (_draggedPin->pinDirection != PinDirection::Out)
                std::swap(origin, target);

            QLinearGradient gradient(origin, target);

            QColor color0 = _draggedPin->color;
            QColor color1 = bThereIsTargetPin ? _draggedPinTargetInfo.value()->color : _draggedPin->color;

            if (_draggedPin->pinDirection == PinDirection::In)
                std::swap(color0, color1);

            gradient.setColorAt(0, color0);
            gradient.setColorAt(1, color1);
            pen.setBrush(QBrush(gradient));
            painter->setPen(pen);

            painter->drawPath(standardPath(origin, target, zoomMult));
        }

        // draw all existing pins connections
        std::ranges::for_each(_connectedPins.keyValueBegin(), _connectedPins.keyValueEnd(),
            [&](std::pair<PinData, PinData> pair) {
            // connections are being drawed from out- to in-pins only
            if (pair.first.pinDirection == PinDirection::In) return;

            QPoint origin = _nodes[pair.first.nodeID]->getOutlineCoordinateForPinID(pair.first.pinID);
            QPoint target = _nodes[pair.second.nodeID]->getOutlineCoordinateForPinID(pair.second.pinID);

            QLinearGradient gradient(origin, target);
            gradient.setColorAt(0, pair.first.color);
            gradient.setColorAt(1, pair.second.color);
            pen.setBrush(QBrush(gradient));
            painter->setPen(pen);

            painter->drawPath(standardPath(origin, target, zoomMult));
        });
    }


    // manage NODEFACTORYWIDGET
    {
        _nfWidget->move(this->width() - 10 - _nfWidget->width(), 10);
        _nfWidget->raise();
    }


    // telemetrics 
    {
        pen.setColor(c_dotsColor);
        painter->setPen(pen);

        auto pointToString = [](QPoint point) {
            return QString::number(point.x()) + ", " + QString::number(point.y());
        };

        auto pointfToString = [](QPointF point) {
            return QString::number(point.x()) + ", " + QString::number(point.y());
        };

        QPoint mouseCanvasPosition = mapToCanvas(_mousePosition.toPoint());
        painter->drawText(QPoint(20, 20), QString( "Mouse on canvas: " + pointToString(mouseCanvasPosition) ));
        painter->drawText(QPoint(20, 40), QString( "Mouse in viewport: " + pointfToString(_mousePosition) ));
        painter->drawText(QPoint(20, 60), QString( "Size: " + QString::number(rectangle.width()) + ", " + QString::number(rectangle.height()) ));
        painter->drawText(QPoint(20, 80), QString( "Center: " + pointfToString(_offset) ));
        painter->drawText(QPoint(20, 100), QString( "Node's canvas pos: " + pointfToString(_nodes[0]->canvasPosition()) ));
        painter->drawText(QPoint(20, 120), QString( "Zoom: " + QString::number(_zoom) ));

        painter->drawText(QPoint(20, 160),
                          QString( "Node's offset: " + pointfToString( zoomMult * (_nodes[0]->canvasPosition() - _offset) + this->rect().center() )));
        painter->drawText(QPoint(20, 180), QString( "Node's pos: " + pointfToString( (_nodes[0]->pos()) ) ));
        painter->drawText(QPoint(20, 200), QString( "Drag pos: " + pointToString(_draggedPinTarget) ));
    }
}

}
