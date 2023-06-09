#include <QtDebug>
#include <QLabel>
#include <QMimeData>
#include <string>

#include "abstractpin.h"
#include "basenode.h"
#include "constants.h"
#include "utility.h"
#include "GraphWidgets/canvas.h"

namespace GraphLib {

AbstractPin::AbstractPin(BaseNode *parent) : AbstractPin(-1, parent)
{}

AbstractPin::AbstractPin(int ID, BaseNode *parent)
    : QWidget{ parent }
    , _parentNode{ parent }
    , _ID{ ID }
    , _color{ QColor(Qt::GlobalColor::black) }
    , _normalD{ c_normalPinD }
    , _bIsConnected{ false }
    , _text{ QString("") }
    , _connectedPins{ QMap<int, PinData>() }
    , _breakConnectionActions{ QMap<int, QAction*>() }
    , _contextMenu{ QMenu(this) }
    , _painter{ new QPainter() }
{
    setAcceptDrops(true);
}

AbstractPin::~AbstractPin() { delete _painter; }


// ------------------- GENERAL ---------------------


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
    if (_connectedPins.contains(ID))
    {
        _connectedPins.remove(ID);
        _bIsConnected = !(_connectedPins.empty());
    }
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
    return PinData(_direction, _parentNode->ID(), _ID);
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

void AbstractPin::startDrag()
{
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

void AbstractPin::showContextMenu(const QMouseEvent *event)
{
    _contextMenu.clear();
    _contextMenu.setTitle(_text);

    QMenu *breakMenu = _contextMenu.addMenu("Break connnection");
    std::ranges::for_each(_connectedPins.asKeyValueRange(), [&](const std::pair<int, PinData> &pair){
        QString nodeName = _parentNode->getParentCanvas()->getNodeName(pair.second.nodeID);
        breakMenu->addAction(new QAction("Break connection to " + nodeName, breakMenu));
        QAction *action = breakMenu->actions().last();
        action->setData(QVariant(pair.second.toByteArray()));

        connect(action, &QAction::triggered, this, [&](){
            QAction *sender = (QAction*)QObject::sender();
            PinData data = PinData::fromByteArray(sender->data().toByteArray());

            PinData out = getDirection() == PinDirection::Out ? getData() : data;
            PinData in  = getDirection() == PinDirection::Out ? data : getData();
            onConnectionBreak(out, in);
        });
    });

    _contextMenu.popup(mapToGlobal(event->pos()));
}


// -------------------- EVENTS ---------------------


void AbstractPin::mousePressEvent(QMouseEvent *event)
{
    switch (event->buttons())
    {
    case Qt::LeftButton:
        startDrag();
        break;
    case Qt::RightButton:
        showContextMenu(event);
        break;
    default:;
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


// -------------------- PAINT ---------------------


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
