#include <QPen>
#include <QRect>
#include <QtDebug>
#include <QApplication>
#include <algorithm>

#include "basenode.h"
#include "GraphWidgets/canvas.h"
#include "utility.h"
#include "constants.h"
#include "GraphWidgets/pin.h"

namespace GraphLib {

BaseNode::BaseNode(Canvas *canvas) : BaseNode(-1, canvas)
{}

BaseNode::BaseNode(int ID, Canvas *canvas)
    : QWidget{ canvas }
    , _parentCanvas{ canvas }
    , _ID{ ID }
    , _zoom{ _parentCanvas->getZoomMultiplier() }
    , _painter{ new QPainter() }
    , _canvasPosition{ QPointF(0, 0) }
    , _hiddenPosition{ QPointF() }
    , _bIsSelected{ false }
    , _lastMouseDownPosition{ QPointF(0, 0) }
    , _mousePressPosition{ QPointF(0, 0) }
    , _name{ QString("") }
    , _pinsOutlineCoords{ QMap<int, QPoint>() }
    , _pins{ QMap<int, AbstractPin*>() }
{
    _normalSize.setWidth(200);
    _normalSize.setHeight(150);

    this->setFixedSize(_normalSize);

    connect(this, &BaseNode::onSelect, this, [this](){
        _bIsSelected = true;
    });
}

BaseNode::~BaseNode()
{
    delete _painter;
    std::ranges::for_each(_pins, [](AbstractPin *pin) { delete pin; });
}

unsigned int BaseNode::IDgenerator = 0;


// ------------------- GENERAL --------------------


void BaseNode::setPinConnection(int pinID, PinData connectedPin)
{
    _pins[pinID]->setConnected(true);
    _pins[pinID]->addConnectedPin(connectedPin);
}

void BaseNode::setPinConnected(int pinID, bool isConnected)
{
    _pins[pinID]->setConnected(isConnected);
}

float BaseNode::getParentCanvasZoomMultiplier() const
{
    return _parentCanvas->getZoomMultiplier();
}

QRect BaseNode::getMappedRect() const
{
    QRect nodeRect = this->rect();
    QPoint mappedTopLeft = this->mapToParent(nodeRect.topLeft());
    QRect mapped(mappedTopLeft.x(), mappedTopLeft.y(), nodeRect.width(), nodeRect.height());
    return mapped;
}

bool BaseNode::hasPinConnections() const
{
    return std::ranges::any_of(_pins, [&](AbstractPin *pin){
        return !pin->getConnectedPins().isEmpty();
    });
}

QSharedPointer< QMap<int, QVector<PinData> > > BaseNode::getPinConnections() const
{
    auto out = QSharedPointer<QMap<int, QVector<PinData> >>(new QMap<int, QVector<PinData> >());
    std::ranges::for_each(_pins, [&](AbstractPin *pin){
        out->insert(pin->ID(), pin->getConnectedPins());
    });
    return out;
}

void BaseNode::removePinConnection(int pinID, int connectedPinID)
{
    _pins[pinID]->removeConnectedPinByID(connectedPinID);
}


// -------------------- SLOTS ---------------------


void BaseNode::addPin(AbstractPin *pin)
{
    pin->setID(newID());
    _pins.insert(pin->ID(), pin);
    connect(pin, &AbstractPin::onDrag, this, &BaseNode::slot_onPinDrag);
    connect(pin, &AbstractPin::onConnect, this, &BaseNode::slot_onPinConnect);
    connect(pin, &AbstractPin::onConnectionBreak, this, &BaseNode::slot_onPinConnectionBreak);
    pin->show();
}

void BaseNode::addPin(QString text, PinDirection direction, QColor color)
{
    int id = BaseNode::newID();
    Pin *newPin = new Pin(id, this);
    _pinsOutlineCoords.insert(id, QPoint(0, 0));
    newPin->setColor(color);
    newPin->setText(text);
    newPin->setDirection(direction);
    addPin(newPin);
}

void BaseNode::slot_onPinDrag(PinDragSignal signal)
{
    switch (signal.type())
    {
    case PinDragSignalType::Start:
        _pins[signal.source().pinID]->setConnected(true);
        break;
    case PinDragSignalType::End:
        _pins[signal.source().pinID]->setConnected(false);
        break;
    default:;
    }
    onPinDrag(signal);
}

void BaseNode::slot_onPinConnect(PinData outPin, PinData inPin)
{ onPinConnect(outPin, inPin); }

void BaseNode::slot_onPinConnectionBreak(PinData outPin, PinData inPin)
{ onPinConnectionBreak(outPin, inPin); }


// -------------------- EVENTS ---------------------


void BaseNode::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        _lastMouseDownPosition = mapToParent(event->position());
        _mousePressPosition = event->position();
        _hiddenPosition = _canvasPosition;
    }
}

void BaseNode::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
    onSelect(event->modifiers() & c_multiSelectionModifier, _ID);
}

void BaseNode::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        if ((event->position() - _mousePressPosition).manhattanLength()
            > QApplication::startDragDistance())
        {
            this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
            if (!_bIsSelected)
                onSelect(event->modifiers() & c_multiSelectionModifier, _ID);
        }

        QPointF offset = mapToParent(event->position()) - _lastMouseDownPosition;
        if (_parentCanvas->getSnappingEnabled())
        {
            _hiddenPosition += (offset / _zoom);
            _canvasPosition = snap(_hiddenPosition, _parentCanvas->getSnappingInterval());
        }
        else
            _canvasPosition += (offset / _zoom);

        _lastMouseDownPosition = mapToParent(event->position());
    }
}


// ----------------- PAINT HELPERS ------------------


void BaseNode::paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    float roundingRadiusZoomed = _zoom * c_nodeRoundingRadius;
    float nameRoundedRectRoundingRadius = roundingRadiusZoomed * 0.1f;



    QRect bounded = painter->boundingRect(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                                          Qt::AlignCenter, _name);

    int oldWidth = bounded.width(), oldHeight = bounded.height();
    bounded.setWidth(bounded.width() * c_nodeNameRoundedRectSizeX);
    bounded.setHeight(bounded.height() * c_nodeNameRoundedRectSizeY);
    bounded.translate((oldWidth - bounded.width()) / 2, (oldHeight - bounded.height()) / 2);

    painter->drawRoundedRect(bounded, nameRoundedRectRoundingRadius, nameRoundedRectRoundingRadius);
}

int BaseNode::calculateRowsOffset(QPainter *painter) const
{
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    return nameBounding.height() * 1.5 + c_normalPinD * _zoom;
}

void BaseNode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    QFont font = standardFont(c_nodeNameSize * _zoom);
    painter->setFont(font);

    painter->drawText(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                      (Qt::AlignVCenter | Qt::AlignHCenter), _name);
}


// --------------------- PAINT ----------------------


void BaseNode::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void BaseNode::paint(QPainter *painter, QPaintEvent *)
{
    _zoom = _parentCanvas->getZoomMultiplier();
    bool bShouldSimplifyRender = _zoom <= c_changeRenderZoomMultiplier;
    int normalPinDZoomed = c_normalPinD * _zoom;

    auto calculateWidth = [&](){
        int maxInWidth = 0, maxOutWidth = 0;

        std::ranges::for_each(_pins, [&](AbstractPin *pin){
            switch (pin->getDirection())
            {
            case PinDirection::In:
                maxInWidth = std::max(maxInWidth, pin->getDesiredWidth(_zoom));
                break;
            case PinDirection::Out:
                maxOutWidth = std::max(maxOutWidth, pin->getDesiredWidth(_zoom));
                break;
            default:;
            }
        });

        return maxInWidth + maxOutWidth + normalPinDZoomed * (bShouldSimplifyRender ? 8 : 4);
    };

    int inPins = std::ranges::count_if(_pins, &AbstractPin::static_isInPin);
    int pinRows = std::max(inPins, static_cast<int>(_pins.size() - inPins));

    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    int pinsOffsetY = calculateRowsOffset(painter);

    int desiredWidth = calculateWidth();
    int desiredHeight = pinsOffsetY + pinRows * normalPinDZoomed * 2;

    _normalSize.setWidth(desiredWidth / _zoom);
    _normalSize.setHeight(desiredHeight / _zoom);

    int outlineWidth = static_cast<int>(std::min(c_globalOutlineWidth * _zoom, c_nodeMaxOutlineWidth));
    int innerWidth = desiredWidth - outlineWidth * 2;
    int innerHeight = desiredHeight - outlineWidth * 2;

    float innerRoundingRadius = _zoom * c_nodeRoundingRadius;

    QPoint desiredOrigin = mapFromParent(QPoint(this->pos()));

    QPainterPath path;

    // paint OUTER
    if (_bIsSelected)
    {
        QPen pen(Qt::SolidLine);
        pen.setColor(c_selectionColor);
        painter->setPen(pen);
    }

    // paint INNER
    {
        painter->setBrush(c_nodesBackgroundColor);

        QRect innerRect(desiredOrigin.x() + outlineWidth, desiredOrigin.y() + outlineWidth, innerWidth, innerHeight);
        path.addRoundedRect(innerRect, innerRoundingRadius, innerRoundingRadius);
        path.setFillRule(Qt::WindingFill);

        painter->drawPath(path);
    }


    // paint NAME
    {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(c_highlightColor);
        painter->setPen(pen);
        painter->setBrush(c_highlightColor);

        const QPoint &textOrigin = desiredOrigin;

        if (bShouldSimplifyRender)
            paintSimplifiedName(painter, desiredWidth, textOrigin);
        else
            paintName(painter, desiredWidth, textOrigin);
    }


    // manage PINS
    {
        pen.setWidth(c_pinConnectLineWidth * _zoom);

        int inPinsOffsetY = pinsOffsetY;
        int outPinsOffsetY = pinsOffsetY;

        auto manage = [&](AbstractPin *pin){
            switch (pin->getDirection())
            {
            case PinDirection::In:
                pin->move(QPoint(normalPinDZoomed, inPinsOffsetY));
                inPinsOffsetY += 2 * normalPinDZoomed;
                break;
            case PinDirection::Out:
                pin->move(QPoint(desiredWidth - normalPinDZoomed - pin->getDesiredWidth(_zoom), outPinsOffsetY));
                outPinsOffsetY += 2 * normalPinDZoomed;
                break;
            default:;
            }

            pin->setFixedSize(pin->getDesiredWidth(_zoom), pin->getNormalD() * _zoom);
            _pinsOutlineCoords[pin->ID()] = QPoint(pin->isInPin() ? 0 : desiredWidth, pin->getCenter().y());

            if (pin->isConnected())
            {
                pen.setColor(pin->getColor());
                painter->setPen(pen);
                painter->drawLine(_pinsOutlineCoords[pin->ID()], pin->getCenter());
            }
        };

        std::ranges::for_each(_pins, manage);
    }
}

}
