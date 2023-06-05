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
    , _painter{ new QPainter() }
    , _canvasPosition{ QPointF(0, 0) }
    , _hiddenPosition{ QPointF() }
    , _bIsSelected{ false }
    , _lastMouseDownPosition{ QPointF(0, 0) }
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

void BaseNode::addPin(AbstractPin *pin)
{
    pin->setID(newID());
    _pins.insert(pin->getID(), pin);
    connect(pin, &AbstractPin::onDrag, this, &BaseNode::slot_onPinDrag);
    connect(pin, &AbstractPin::onConnect, this, &BaseNode::slot_onPinConnect);
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

void BaseNode::slot_onPinConnect(PinData sourcePin, PinData targetPin)
{ onPinConnect(sourcePin, targetPin); }

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

void BaseNode::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        _lastMouseDownPosition = mapToParent(event->position());
        _hiddenPosition = _canvasPosition;
    }
    else event->ignore();
}

void BaseNode::mouseReleaseEvent(QMouseEvent *event)
{
    this->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
    onSelect(event->modifiers() & c_multiSelectionModifier, _ID);
    event->ignore();
}

void BaseNode::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::MouseButton::LeftButton)
    {
        if ((mapToParent(event->position()) - _lastMouseDownPosition).manhattanLength()
            > 0.2f * QApplication::startDragDistance())
        {
            this->setCursor(QCursor(Qt::CursorShape::OpenHandCursor));
            if (!_bIsSelected)
                onSelect(event->modifiers() & c_multiSelectionModifier, _ID);
        }

        QPointF offset = mapToParent(event->position()) - _lastMouseDownPosition;
        if (_parentCanvas->getSnappingEnabled())
        {
            _hiddenPosition += (offset / _parentCanvas->getZoomMultiplier());
            _canvasPosition = snap(_hiddenPosition, _parentCanvas->getSnappingInterval());
        }
        else
            _canvasPosition += (offset / _parentCanvas->getZoomMultiplier());

        _lastMouseDownPosition = mapToParent(event->position());
    }
    else event->ignore();
}

void BaseNode::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void BaseNode::paintSimplifiedName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);
    float roundingRadiusZoomed = _parentCanvas->getZoomMultiplier() * c_nodeRoundingRadius;
    float nameRoundedRectRoundingRadius = roundingRadiusZoomed * 0.1f;



    QRect bounded = painter->boundingRect(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                                          Qt::AlignCenter, _name);

    int oldWidth = bounded.width(), oldHeight = bounded.height();
    bounded.setWidth(bounded.width() * c_nodeNameRoundedRectSizeX);
    bounded.setHeight(bounded.height() * c_nodeNameRoundedRectSizeY);
    bounded.translate((oldWidth - bounded.width()) / 2, (oldHeight - bounded.height()) / 2);

    painter->drawRoundedRect(bounded, nameRoundedRectRoundingRadius, nameRoundedRectRoundingRadius);
}

void BaseNode::paintName(QPainter *painter, int desiredWidth, QPoint textOrigin)
{
    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);

    painter->drawText(QRect(textOrigin.x(), textOrigin.y(), desiredWidth, nameBounding.height() * 2),
                      (Qt::AlignVCenter | Qt::AlignHCenter), _name);
}

void BaseNode::paint(QPainter *painter, QPaintEvent *)
{
    float parentZoom = _parentCanvas->getZoomMultiplier();
    bool bShouldSimplifyRender = parentZoom <= c_changeRenderZoomMultiplier;
    int normalPinDZoomed = c_normalPinD * parentZoom;

    auto calculateWidth = [&](){
        int maxInWidth = 0, maxOutWidth = 0;

        std::ranges::for_each(_pins, [&](AbstractPin *pin){
            switch (pin->getDirection())
            {
            case PinDirection::In:
                maxInWidth = std::max(maxInWidth, pin->getDesiredWidth(parentZoom));
                break;
            case PinDirection::Out:
                maxOutWidth = std::max(maxOutWidth, pin->getDesiredWidth(parentZoom));
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
    QFont font = standardFont(c_nodeNameSize * parentZoom);
    painter->setFont(font);

    QSize nameBounding = painter->fontMetrics().size(Qt::TextSingleLine, _name);

    int pinsOffsetY =  nameBounding.height() * 1.5 + normalPinDZoomed;

    int desiredWidth = calculateWidth();
    int desiredHeight = pinsOffsetY + pinRows * normalPinDZoomed * 2;

    _normalSize.setWidth(desiredWidth / parentZoom);
    _normalSize.setHeight(desiredHeight / parentZoom);

    int outlineWidth = static_cast<int>(std::min(c_globalOutlineWidth * parentZoom, c_nodeMaxOutlineWidth));
    int innerWidth = desiredWidth - outlineWidth * 2;
    int innerHeight = desiredHeight - outlineWidth * 2;

    float innerRoundingRadius = parentZoom * c_nodeRoundingRadius;

    // uncomment these for the outer
    // 0.92 is a magic number
//    float innerRoundingRadius = roundingRadiusZoomed * 0.92;
//    float outerRoundingRadius = roundingRadiusZoomed;



    QPoint desiredOrigin = mapFromParent(QPoint(this->pos()));

    QPainterPath path;

    // paint OUTER
    if (_bIsSelected)
    {
        QPen pen(Qt::SolidLine);
        pen.setColor(c_selectionColor);
        painter->setPen(pen);

//        QRect outerRect(desiredOrigin.x(), desiredOrigin.y(), desiredWidth, desiredHeight);
//        path.addRoundedRect(outerRect, outerRoundingRadius, outerRoundingRadius);

//        painter->drawPath(path);
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
        pen.setColor(c_nodesOutlineColor);
        painter->setPen(pen);
        painter->setBrush(c_nodesOutlineColor);

        const QPoint &textOrigin = desiredOrigin;

        if (bShouldSimplifyRender) // draw rounded rect instead of text
            paintSimplifiedName(painter, desiredWidth, textOrigin);
        else // draw actual text
            paintName(painter, desiredWidth, textOrigin);
    }


    // manage PINS
    {
        pen.setWidth(c_pinConnectLineWidth * parentZoom);

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
                pin->move(QPoint(desiredWidth - normalPinDZoomed - pin->getDesiredWidth(parentZoom), outPinsOffsetY));
                outPinsOffsetY += 2 * normalPinDZoomed;
                break;
            default:;
            }

            pin->setFixedSize(pin->getDesiredWidth(parentZoom), pin->getNormalD() * parentZoom);
            _pinsOutlineCoords[pin->getID()] = QPoint(pin->isInPin() ? 0 : desiredWidth, pin->getCenter().y());

            if (pin->isConnected())
            {
                pen.setColor(pin->getColor());
                painter->setPen(pen);
                painter->drawLine(_pinsOutlineCoords[pin->getID()], pin->getCenter());
            }
        };

        std::ranges::for_each(_pins, manage);
    }
}

}
