
#include "abstractpin.h"
#include "constants.h"
#include "node.h"
#include <QtDebug>
#include <QLabel>
#include <QMimeData>
#include <string>
#include "util_functions.h"

namespace GraphLib {

AbstractPin::AbstractPin(int ID, Node *parentNode, QWidget *parent)
    : QWidget{ parent }
    , _parentNode{ parentNode }
    , _ID{ ID }
    , _color{ QColor(Qt::GlobalColor::black) }
    , _normalD{ c_normalPinD }
    , _bIsConnected{ false }
    , _text{ QString("") }
    , _connectedPins{ QMap<int, PinData>() }
    , _painter{ new QPainter() }
{
    setAcceptDrops(true);
}

AbstractPin::~AbstractPin() { delete _painter; }

void AbstractPin::setConnected(bool isConnected)
{
    if (_connectedPins.isEmpty())
        _bIsConnected = isConnected;
    else
        _bIsConnected = true;
}

void AbstractPin::addConnectedPin(PinData pin)
{
    if (pin.pinDirection == _direction)
        throw std::invalid_argument("AbstractPin::addConnectedPin - pin with the same direction passed as the argument.");
    _connectedPins.insert(pin.pinID, pin);
    _bIsConnected = true;
}

void AbstractPin::removeConnectedPinByID(int ID)
{
    _connectedPins.remove(ID);
    _bIsConnected = !(_connectedPins.empty());
}

int AbstractPin::getNodeID() const
{
    return _parentNode->ID();
}

QPixmap AbstractPin::getPixmap() const
{
    int width = 0.5 * c_normalPinD * _parentNode->getParentCanvasZoomMultiplier();
    QPixmap pixmap(QSize(width, width));
    pixmap.fill(QColor(255, 255, 255, 0));

//    _painter->begin(&pixmap);
//    _painter->setBrush(_color);
//    _painter->setPen(QPen(Qt::NoPen));
//    _painter->drawEllipse(QRect(0, 0, width, width));
//    _painter->end();
    return pixmap;
}

PinData AbstractPin::getData() const
{
    return PinData(_direction, _parentNode->ID(), _ID, _text, _color);
}

void AbstractPin::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setData(c_mimeFormatForPinConnection, this->getData().toByteArray());
        drag->setMimeData(mimeData);

        QPixmap pixmap = getPixmap();
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));

        onDrag(PinDragSignal(getData(), PinDragSignalType::Start));
        drag->exec();
        onDrag(PinDragSignal(getData(), PinDragSignalType::End));
    }
}

void AbstractPin::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForPinConnection))
    {
        event->setDropAction(Qt::LinkAction);
        event->acceptProposedAction();
        QByteArray byteArray = event->mimeData()->data(c_mimeFormatForPinConnection);
        PinData sourceData = PinData::fromByteArray(byteArray);

        if (_direction == PinDirection::Out)
            onConnect(getData(), sourceData);
        else
            onConnect(sourceData, getData());
    }
}

void AbstractPin::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(c_mimeFormatForPinConnection))
    {
        PinData data = PinData::fromByteArray(event->mimeData()->data(c_mimeFormatForPinConnection));
        PinDirection sourceDirection = data.pinDirection;
        int sourceNodeID = data.nodeID;
        if (sourceDirection != _direction && sourceNodeID != _parentNode->ID())
        {
            event->setDropAction(Qt::LinkAction);
            event->acceptProposedAction();
            onDrag(PinDragSignal(getData(), PinDragSignalType::Enter));
            return;
        }
    }
}

void AbstractPin::dragLeaveEvent(QDragLeaveEvent *)
{
    onDrag(PinDragSignal(getData(), PinDragSignalType::Leave));
}

int AbstractPin::getDesiredWidth(float zoom) const
{
    if (_text == "" || zoom <= c_changeRenderZoomMultiplier)
        return _normalD * zoom;

    QFont font = standardFont(static_cast<int>(_normalD * zoom * c_pinFontSizeCoef));
    QFontMetrics metrics(font);
    int textWidth = metrics.size(Qt::TextSingleLine, _text).width();
    return static_cast<int>(textWidth + _normalD * 2 * zoom);
}

void AbstractPin::paintEvent(QPaintEvent *event)
{
    _painter->begin(this);
    _painter->setRenderHint(QPainter::Antialiasing, true);
    paint(_painter, event);
    _painter->end();
}

void AbstractPin::paint(QPainter *painter, QPaintEvent *)
{

    float canvasZoom = _parentNode->getParentCanvasZoomMultiplier();

    // D stands for diameter
    int desiredD = _normalD * canvasZoom;

    QPen pen(Qt::NoPen);
    painter->setPen(pen);
    QFont font = standardFont(desiredD * c_pinFontSizeCoef);
    painter->setFont(font);
    painter->setBrush(_color);

    int outlineWidth = c_globalOutlineWidth * canvasZoom;

    QPoint desiredOrigin = mapFromParent(QPoint(this->pos()));
    QPoint textOrigin = QPoint(desiredOrigin.x() + desiredD * 2, desiredOrigin.y());

    QRect rectangle = QRect(desiredOrigin.x(), desiredOrigin.y(), desiredD, desiredD);

    QSize textBounding = painter->fontMetrics().size(Qt::TextSingleLine, _text);



    if (_direction == PinDirection::Out && !(canvasZoom <= c_changeRenderZoomMultiplier))
    {
        rectangle.setX(this->width() - desiredD);
        rectangle.setWidth(desiredD);
    }

    _center = rectangle.center();


    // Drawing main circle
    painter->drawEllipse(rectangle);

    // If needed, draw inner circle the color of the background
    if (!(_bIsConnected || canvasZoom <= c_changeRenderZoomMultiplier))
    {
        painter->setBrush(c_nodesBackgroundColor);
        QRect innerRect = QRect(rectangle.x() + outlineWidth
                              , rectangle.y() + outlineWidth
                              , rectangle.width() - outlineWidth * 2
                              , rectangle.height() - outlineWidth * 2);

        painter->drawEllipse(innerRect);
    }


    // Text-related stuff
    if (_text != "" && !(canvasZoom <= c_changeRenderZoomMultiplier))
    {
        pen.setStyle(Qt::SolidLine);
        pen.setColor(_color);
        painter->setPen(pen);

        if (_direction == PinDirection::Out)
            textOrigin.setX(this->width() - desiredD * 2 - textBounding.width());

        painter->drawText(QRect(textOrigin.x(), textOrigin.y(), textBounding.width(), rectangle.height())
                          , Qt::AlignVCenter, _text);
    }
}

}
