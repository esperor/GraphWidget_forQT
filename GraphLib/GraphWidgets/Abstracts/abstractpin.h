
#pragma once

#include <QObject>
#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFont>
#include <QFontMetrics>
#include <QDrag>
#include <QList>
#include <QByteArray>
#include <QDropEvent>
#include <QMenu>

#include "GraphLib_global.h"
#include "DataClasses/pindata.h"
#include "DataClasses/pindragsignal.h"

namespace GraphLib {

class BaseNode;

enum class PinDirection
{
    In,
    Out,
};

class GRAPHLIB_EXPORT AbstractPin : public QWidget
{
    Q_OBJECT

public:
    AbstractPin(BaseNode *parent);
    AbstractPin(int ID, BaseNode *parent);
    ~AbstractPin();

    void setID(int newID) { _ID = newID; }
    void setConnected(bool isConnected);
    void setColor(QColor color) { _color = color; }
    void setNormalD(float newD) { _normalD = newD; }
    void setText(QString text) { _text = text; }
    void setDirection(PinDirection dir) { _direction = dir; }
    void addConnectedPin(PinData pin);
    void removeConnectedPinByID(int ID);

    int ID() const { return _ID; }
    int getNodeID() const;
    bool isConnected() const { return _bIsConnected; }
    const QColor &getColor() const { return _color; }
    const float &getNormalD() const { return _normalD; }
    QString getText() const { return _text; }
    int getDesiredWidth(float zoom) const;
    PinDirection getDirection() const { return _direction; }
    bool isInPin() const { return _direction == PinDirection::In; }
    QPoint getCenter() const { return mapToParent(_center); }
    QPixmap getPixmap() const;
    PinData getData() const;

    // int here is pinID of connected pin
    QVector<PinData> getConnectedPins() const { return _connectedPins.values(); }

    static bool static_isInPin(const AbstractPin *pin) { return pin->getDirection() == PinDirection::In; }

signals:
    void onDrag(PinDragSignal signal);
    void onConnect(PinData outPin, PinData Pin);
    void onConnectionBreak(PinData outPin, PinData Pin);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private:
    void paint(QPainter *painter, QPaintEvent *event);
    void startDrag();
    void showContextMenu(const QMouseEvent *event);

    BaseNode *_parentNode;
    int _ID;
    QColor _color;
    float _normalD;
    bool _bIsConnected;
    QString _text;
    PinDirection _direction;
    QPoint _center;
    // int here is pinID of connected pin
    QMap<int, PinData> _connectedPins;
    QMap<int, QAction*> _breakConnectionActions;

    QMenu _contextMenu;

    QPainter *_painter;
};

}

